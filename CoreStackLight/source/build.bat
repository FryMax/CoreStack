cls

echo ================================================================BUILD

em++ -std=c++14                                             ^
	./Main.cpp												^
	../../CoreShared/include/gui/imimpl/imgui_impl_SDL2_GL3.cpp^
	../../3rd-party/imgui-1.70/imgui.cpp					^
	../../3rd-party/imgui-1.70/imgui_demo.cpp				^
	../../3rd-party/imgui-1.70/imgui_draw.cpp				^
	../../3rd-party/imgui-1.70/imgui_widgets.cpp			^
	../../3rd-party/fmt-6.0.0/src/format.cc					^
	../../3rd-party/fmt-6.0.0/src/posix.cc					^
	-o ./em/page.html										^
	-I ../../												^
	-I ../../SimpleLib/include								^
	-I ../../3rd-party/imgui-1.70							^
	-I ../../3rd-party/fmt-6.0.0/include					^
	-I ../../3rd-party/glm-0.9.9.5/glm						^
	-s "EXPORTED_FUNCTIONS=['_emHelloJavaScript', '_main']"	^
	-s "EXPORTED_FUNCTIONS=['_emSetArticleWidth', '_main']"	^
	-s USE_WEBGL2=1											^
	-s FULL_ES2=1											^
	-s FULL_ES3=1 											^
	-s USE_SDL=2 											^
	-s ALLOW_MEMORY_GROWTH=1         ^
	-s SAFE_HEAP=1                   ^
	-s ALIASING_FUNCTION_POINTERS=0  ^
	-s ASSERTIONS=2                  ^
	-s DEMANGLE_SUPPORT=1

echo ================================================================RUN

emrun ./em/page.html

	-s "EXTRA_EXPORTED_RUNTIME_METHODS=['ccall', 'cwarp']"	^

	-s DEMANGLE_SUPPORT=1
	-s ASSERTIONS=2
	-s WASM=1

	-O2
	-O3
	--preload-file ASSETS/




