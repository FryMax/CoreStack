#pragma once
#include "../shared_in.h"

LIB_NAMESPACE_START

namespace net
{
	//--------------------------------------------
	struct net_impl;
	struct net_statistic
	{
		uint64_t bytes_resived      = 0;
		uint64_t bytes_sended       = 0;
		uint64_t datagramms_resived = 0;
		uint64_t datagramms_sended  = 0;
	};

	class UdpSocket : utka::uclass
	{
	private:
		std::atomic_bool          m_active = false;
		std::unique_ptr<net_impl> m_impl = nullptr;

		std::vector<std::string>  m_resive_buffer;
		std::mutex                m_resive_buffer_guard;

		net_statistic             m_statistic;

        std::mutex                m_close_guard;


	public:
		virtual ~UdpSocket();
		UdpSocket();

		bool ListenOnPort(uint16_t port);
		void Close();
		void SendDatagrammTo(const std::string& packet, const std::string& ip, uint16_t port);

		net_statistic getStatistic() const;
		std::vector<std::string> popResivedData();//todell

	protected:
		void receive_loop();
	};

	//--------------------------------------------
	struct FlexBuffer
	{
	protected:
		//uint64_t buffer_size = 0;
		std::vector<byte> buffer;

	public:
		size_t buffer_size() const
		{
			return buffer.size();
		}

		void reserve(const size_t size)
		{
			buffer.reserve(size);
		}

		template<typename T>
		void write(const T& var)
		{
			write(reinterpret_cast<const byte_t*>(&var), sizeof(T));
		}

		void write(const byte* data, const size_t size)
		{
			BREAK_IF(!data);
			BREAK_IF(size == 0);

			//todo mb made custom allocator
			//todo if size == 0 warn and return;

			const size_t offset = buffer_size();

			buffer.resize(offset + size);
			std::memcpy(&buffer[offset], data, size);
		}

		//template<typename T>
		const byte* read_at(const size_t offset) const
		{
			return reinterpret_cast<const byte*>(&buffer[offset]);
		}

		//template<typename T>
		byte* read_at_mut(const size_t offset)
		{
			return reinterpret_cast<byte*>(&buffer[offset]);
		}
	};

	//---------------------------------------------------------------
	//enum sunc_data_type
	//{
	//	//abolish basic objects
	//	pod_type,
	//	partible_type,
	//};

	struct Packet
	{
	public:
		Packet() : data_size(0), sunc_begin(0), sunc_total(0) {}

		Packet(FlexBuffer&& buff, uint64_t total = 0, uint64_t offset = 0, uint64_t size = 0) :
			//data_type(partible_type),
			data_size(total == 0 ? buff.buffer_size() : total),
			sunc_begin(offset),
			sunc_total(size == 0 ? data_size : size),
			data(std::move(buff))
		{
			DEBUG_BREAK_IF(data.buffer_size() == 0);
		}

		//[8/type][8/compressed]

	public:
		// full packet or part?
		// full packet id?
		// compressed?
		uint64_t data_size;  //
		uint64_t sunc_begin; //
		uint64_t sunc_total; //

		FlexBuffer data;
	};

	//---------------------------------------------------------------

	struct ISerializer
	{
		// return true if packet contains data
		virtual bool implSerialize(Packet& packet) const { return false; }

		// return true if packet processed
		virtual bool implDeserialize(Packet&& packet) { return false; }

	protected:
		//bool m_process_sub_sunc = false;
		//virtual bool process_unresived() { return false; };
	};

	template<typename T>
	struct PodSerializer : ISerializer
	{
	private:
		T* const pVariable;
		mutable T prevValue;
		const bool need_delete;

	public:
		//store_t() : storedValue(new T(0)), comparator(storedValue), need_delete(true)  {}
		PodSerializer(T* const var) : pVariable(var), prevValue(*var),   need_delete(false)
		{
		}

		~PodSerializer()
		{
			if (need_delete)
				if (pVariable)
					delete pVariable;
				else
					BREAK_IF(true);
		}

	public:
		bool implSerialize(Packet& out) const override
		{
			// fetch
			//---------------------------
			if (prevValue == *pVariable)
				return false;

			prevValue = *pVariable;

			// process
			//---------------------------
			//TODO: custom mem allocator
			//TODO: custom fill impl?
			//TODO: byte order

			FlexBuffer fb;
			fb.write<T>(*pVariable);
			out = Packet(std::move(fb));

			return true;
		}

		virtual bool implDeserialize(Packet&& in) override
		{
			BREAK_IF(in.sunc_begin != 0);
			BREAK_IF(in.data_size  != sizeof(T));
			BREAK_IF(in.sunc_total != sizeof(T));

			const FlexBuffer& fb = in.data;
			//TODO: byte order:convert from big-endian?

			const auto pData = reinterpret_cast<const T*>(fb.read_at(0));
			prevValue = *pVariable = *pData;

			return true;
		}
	};

	//---------------------------------------------------------------
	// todo mb change se from to OOP->FP
	//
	//
	//---------------------------------------------------------------

	using Int32Serializer = PodSerializer<int32_t>;

 	struct SuncPacket
	{
		//resiver(resivers?)
		UID id;

		// data-sunc sub-packet
		Packet data;
	};

	struct DatagramHeader
	{
		uint64_t total_size;

		//todo md resive_approve_expected
		//todo md add magic
		//todo md add version
		//todo md add checksum

		//std::vector<sunc_packet> sunc_packets;
	};

	class ISuncWarper
	{
	public:
		virtual void implSendSunc(const std::vector<SuncPacket>& packets)
		{
			LOG_WARN_NO_IMPL;
		}

		virtual std::vector<SuncPacket> implGetSuncCollection()
		{
			LOG_WARN_NO_IMPL;
			return {};
		}
	};

	class SuncManager
	{
	public:
		bool Register(const UID& uid, ISerializer* const pInstance)
		{
			//ADD MUTEX
			const auto it = serializers.find(uid);
			if (it != serializers.end())
			{
				log::warn << "uid occouped";
				BREAK_IF(true);
				return false;
			}

			serializers.emplace_hint(it, std::make_pair(uid,pInstance));
			return true;
		}

		//template<typename T, cmpT>
		//void Register(const smUID& uid, const smVar<T, cmpT>& inst)
		//{
		//
		//}

		void Unregister(const UID& uid)
		{
			LOG_WARN_NO_IMPL;
		}

		bool SendSunc(ISuncWarper& suncWarper)
		{
			send_buffer.clear();
			send_buffer.reserve(serializers.size());

			// collect
			//--------------------------------------------
			for (const auto& it : serializers)
			{
				Packet buff;
				if (it.second->implSerialize(buff))
					send_buffer.emplace_back(SuncPacket{ it.first, std::move(buff) });
			}

			// send to warper
			//--------------------------------------------
			suncWarper.implSendSunc(send_buffer);

			return !send_buffer.empty();
		}

		bool ReadSunc(ISuncWarper& sunc_warper)
		{
			read_buffer.clear();
			read_buffer.reserve(serializers.size());

			// get resived buffer
			//--------------------------------------------
			read_buffer = std::move(sunc_warper.implGetSuncCollection());

			// serialize all messages
			//--------------------------------------------
			for (auto& it : read_buffer)
			{
				const auto item = serializers.find(it.id);
				if (item != serializers.end())
				{
					item->second->implDeserialize(std::move(it.data));
				}
				else
				{
					log::debug << "unknown-uid object sunc skipped";
				}
			}

			return !read_buffer.empty();
		}

	protected:
		std::map<UID, ISerializer* const> serializers;

	private:
		std::vector<SuncPacket> send_buffer;
		std::vector<SuncPacket> read_buffer;
	};


	////////////////////////////////////////////////////////////////////////
	class Mutex
	{
		std::atomic<bool> flag{ false };

	public:
		void lock()
		{
			while (flag.exchange(true, std::memory_order_relaxed));
			std::atomic_thread_fence(std::memory_order_acquire);
		}

		void unlock()
		{
			std::atomic_thread_fence(std::memory_order_release);
			flag.store(false, std::memory_order_relaxed);
		}
	};
	class Lock
	{
	public:
		Lock(Mutex& m) : mut(m) { mut.lock(); }
		~Lock() { mut.unlock(); }

	private:
		Mutex& mut;
	};
	////////////////////////////////////////////////////////////////////////

	class SuncPipe : public ISuncWarper
	{
		UdpSocket sock;
		int src_port = 5000;
		int out_port = 6000;

	public:
		SuncPipe()
		{
			if (!sock.ListenOnPort(src_port))
			{
				std::swap(src_port, out_port);
				BREAK_IF(!sock.ListenOnPort(src_port));
			}
		}

		auto getStat() const
		{
			return sock.getStatistic();
		}

	public:
		void implSendSunc(const std::vector<SuncPacket>& packets) override
		{
			if (packets.size() == 0)
				return;

			FlexBuffer datagram;

			//todo sunc message header
			{
				DatagramHeader dummy_header{000000};
				datagram.write(dummy_header);
			}

			DatagramHeader* header = reinterpret_cast<DatagramHeader*>(datagram.read_at_mut(0));

			for (const auto& it : packets)
			{
				// reciver
				datagram.write(it.id);

				// data
				const auto& fragment = it.data;
				datagram.write(fragment.data_size);
				datagram.write(fragment.sunc_begin);
				datagram.write(fragment.sunc_total);
				datagram.write(fragment.data.read_at(0), fragment.data.buffer_size());
			}

			const size_t MAX_OUTPUT_BUFFER_SIZE = 1500;

			//TODO:
			//**control big objects
			//**control buffer`s growth
			//**process buffer`s async

			// [5]
			// [500]
			// [------2000------]
			// [500]
			// [--------5000--------]
			// [5]
			// [a:0:9][b:9:4]
			// [5][500][      5000      ][500]
			// [         ][         ][       ]
			// [id,data,0,999][id,data,0,999][id,data,0,999]
			// [size][cmp A][size][cmp B]
			//compress

			// [compressed data]
			// [1500][1500][1500][1500][1500][1500][1500][1500][1500][1500]
			// [1500][1500][756]

			// 0[1500]
			// 1[1500]
			// 2[756]

			//if (datagram.buffer_size() > 0)
			{
				pushDatagram(std::move(datagram));
			}
		}

		std::vector<SuncPacket> implGetSuncCollection() override
		{
			std::vector<SuncPacket> collection;

			//if (has input packets)
			{
				std::vector<FlexBuffer> datagrams = popDatagrams();

				if (!datagrams.empty())
				{
					// [{4}-...] - [{4}-...]
					for (const auto& it : datagrams)
					{
						auto& header = *reinterpret_cast<const DatagramHeader*>(it.read_at(0));

						// // reciver
						// datagram.write(it.id);
						//
						// // data
						// const auto& fragment = it.data;
						// datagram.write(fragment.data_size);
						// datagram.write(fragment.sunc_begin);
						// datagram.write(fragment.sunc_total);
						// datagram.write(fragment.data.read_at(0), fragment.data.buffer_size());

						// [128][64][64][...] - [128][64][64][...]

						size_t offset = 0;
						offset += sizeof(DatagramHeader);

						while (offset < it.buffer_size())
						{
							SuncPacket fragment;

							const size_t offset_uid   = offset = offset + 0;
							const size_t offset_size  = offset = offset + sizeof(UID);
							const size_t offset_begin = offset = offset + sizeof(uint64_t);
							const size_t offset_total = offset = offset + sizeof(uint64_t);
							const size_t offset_data  = offset = offset + sizeof(uint64_t);

							fragment.id              = *(     UID*)it.read_at(offset_uid);
							fragment.data.data_size  = *(uint64_t*)it.read_at(offset_size);
							fragment.data.sunc_begin = *(uint64_t*)it.read_at(offset_begin);
							fragment.data.sunc_total = *(uint64_t*)it.read_at(offset_total);

							offset = offset + fragment.data.data_size;
							fragment.data.data.write(it.read_at(offset_data), fragment.data.data_size);

							collection.emplace_back(std::move(fragment));
						}
					}
				}
			}

			return collection;
		}

		//std::atomic_bool has_data = false;
		//std::atomic_bool key_lend = false;
		//std::unique_ptr<se_buffer> last_sunc;
		//std::deque<se_buffer> send_queue;
		//mutable Mutex push_key;

		//------------ for debug only
		//size_t pack_buff_cout () { return push_buff.size(); }
		//size_t pack_total_size() { size_t bytes = 0; for (const auto& it : push_buff) bytes += it.buffer_size(); return bytes; }

		// out -> in
		void push_to_pop()
		{
			const std::lock_guard<std::mutex> lockA(push_guard);
			const std::lock_guard<std::mutex> lockB(resive_guard);
			resive_buff = std::move(push_buff);
		}
		//------------

	protected:
		void pushDatagram(FlexBuffer&& datagram)
		{
			DEBUG_BREAK_IF(datagram.buffer_size() == 0);
			push_guard.lock();
			push_buff.push_back(std::move(datagram));
			push_guard.unlock();

			// TODO: ASYNC THIS
			//-----------------
			for (const auto& it : push_buff)
			{
				std::string str((char*)it.read_at(0), it.buffer_size());
				sock.SendDatagrammTo(str, "127.0.0.1", out_port);
			}
			push_buff.clear();
		}

		std::vector<FlexBuffer> popDatagrams()
		{
			const auto datagrams = sock.popResivedData();
			const std::lock_guard<std::mutex> lock(resive_guard);
			// TODO: ASYNC THIS
			//-----------------
			resive_buff.clear();
			for (const auto& it : datagrams)
			{
				FlexBuffer buff;
				buff.write((byte_t*)it.c_str(), it.size());
				resive_buff.push_back(std::move(buff));
			}
			return std::move(resive_buff);
		}

	private:
		//----------------------------------- send buffer
		std::mutex push_guard;
		std::vector<FlexBuffer> push_buff;

		//----------------------------------- resive buffer
		std::mutex resive_guard;
		std::vector<FlexBuffer> resive_buff;

		//void send_pool()
		//{
		//	while (send_active)
		//	{
		//		// if (new_sms)
		//		{
		//			decltype(push_buff) buff;
		//			{
		//				const std::lock_guard<std::mutex> lock(push_guard);
		//				std::swap(push_buff, buff);
		//			}
		//
		//			for (auto& it : buff)
		//			{
		//				//  if (buffer_very_fat)
		//				//    fat_object_queue-> push_buff
		//				//  else
		//				send_queue.push_front(std::move(it));
		//			}
		//		}
		//
		//
		//	}
		//
		//	}
		//
		//	{
		//		const std::lock_guard<std::mutex> lock(push_guard);
		//		for (auto& it : push)
		//			send_queue.push_front(std::move(it));
		//		push.clear();
		//	}
		//
		//	while (!send_queue.empty())
		//	{
		//	}
		//	send_queue
		//	push_key.unlock();
		//	//each 000
		//	{
		//		// if has sms
		//		//    send sms
		//	}
		//}
		//void read_pool()
		//{
		//	//each
		//	//
		//	// read sms
		//
		//}
	};




}

LIB_NAMESPACE_END