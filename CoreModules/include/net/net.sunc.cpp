//#include "../shared_in.h"
#include "net.sunc.h"

#define ASIO_STANDALONE
#define ASIO_NO_DEPRECATED
#include <3rd-party/asio-1.13.0/include/asio.hpp>
using namespace asio;

LIB_NAMESPACE_START

namespace net
{
	struct net_impl
	{
		net_impl() : udp_sock(context) {}

		asio::io_context context;
		asio::ip::udp::socket udp_sock;
	};

    net::UdpSocket::~UdpSocket()
    {
        Close();
    };

	UdpSocket::UdpSocket() : UCLASS("Udp socket"), m_impl(std::make_unique<net_impl>())
	{

	};

	//------------------------------------------------------------------------
	bool UdpSocket::ListenOnPort(uint16_t port)
	{
		if (m_active)
		{
			log::warn << str::format::insert(__FUNCTION__" failed: already active");
			return false;
		}

		try
		{
			m_impl->udp_sock = asio::ip::udp::socket(m_impl->context, asio::ip::udp::endpoint(asio::ip::udp::v4(), port));
			m_active = true;

			std::thread resiveloop(&UdpSocket::receive_loop, this);
			resiveloop.detach();

			log::debug << str::format::insert("udp-socket binded on [{}]", port);
			return true;
		}
		catch (const std::exception& e)
		{
			log::error << std::string(__FUNCTION__) + e.what();
		}
		catch (...)
		{
			log::error << __FUNCTION__;
		}

		Close();
		return false;
	}

	//------------------------------------------------------------------------
	void UdpSocket::Close()
	{
		const std::lock_guard<std::mutex> lock(m_close_guard);

		if (m_active.exchange(false))
		{
			m_impl->udp_sock.shutdown(asio::socket_base::shutdown_type::shutdown_both);
			m_impl->udp_sock.close();
			log::debug << str::format::insert("[socket] closed");
		}
		else
		{
			log::debug << str::format::insert("[socket] close skipped: not active");
		}
	}

	//------------------------------------------------------------------------

	void UdpSocket::SendDatagrammTo(const std::string& data, const std::string& ip, uint16_t port)
	{
		if (!m_impl->udp_sock.is_open()) { m_impl->udp_sock.open(asio::ip::udp::v4()); }

		asio::ip::udp::endpoint dest(asio::ip::make_address(ip), port);
		m_impl->udp_sock.send_to(asio::buffer(data), dest);

		m_statistic.bytes_sended += data.size();
		m_statistic.datagramms_sended++;
	}

	//------------------------------------------------------------------------
	void UdpSocket::receive_loop()
	{
		const auto receive_action = [this](std::string&& msg, const asio::ip::udp::endpoint& sender)
		{
			const std::string view = msg.size() > 32 ? msg.substr(0, 32) + "..." : msg;

			//log::info << str::format::insert("[socket] Resived {} bytes from ({}): {}",
			//	msg.size(), sender.address().to_string(), view);

			const std::lock_guard<std::mutex> lock(m_resive_buffer_guard);
			m_resive_buffer.push_back(std::move(msg));

			// SendPacketTo(NetPack{ std::string("resived: '" + sms + "'") },
			//	 sender.address().to_string(),
			//	 sender.port());
		};

		try
		{
			char buff[2048];
			asio::ip::udp::endpoint	sender;

			while (m_active)
			{
				const size_t nof_bytes = m_impl->udp_sock.receive_from(asio::buffer(buff), sender);
				receive_action(std::move(std::string(buff, nof_bytes)), sender);

				m_statistic.bytes_resived += nof_bytes;
				m_statistic.datagramms_resived++;
			}
		}
		catch (const std::exception& e)
		{
			log::error << std::string(__FUNCTION__) + e.what();
		}
		catch (...)
		{
			log::error << __FUNCTION__;
		}
	}

	//------------------------------------------------------------------------
	std::vector<std::string> UdpSocket::popResivedData()
	{
		std::vector<std::string> out{};

		if (!m_resive_buffer.empty())
		{
			const std::lock_guard<std::mutex> lock(m_resive_buffer_guard);
			out = std::move(m_resive_buffer);
		}

		return out;
	}

	//------------------------------------------------------------------------
	net_statistic UdpSocket::getStatistic() const
	{
		return m_statistic;
	}

	void gen()
	{
	}

	// </UdpSocket>
}

LIB_NAMESPACE_END