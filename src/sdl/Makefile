GPP := g++-11 -std=c++17

#GPP += -O3
#GPP += -DNDEBUG

# apt-get install libsdl2-dev
SDL2 := `sdl2-config --cflags --libs`
TBB := -ltbb

ALL_LFLAGS := $(SDL2) $(TBB)

root  := ../..
build := $(root)/build/sdl
src   := $(root)/src
util  := $(src)/util
input := $(src)/input
sdl   := $(src)/sdl

exe := sdl_app

program_exe := $(build)/$(exe)

stopwatch_h := $(util)/stopwatch.hpp

types_h := $(util)/types.hpp

button_state_h := $(input)/button_state.hpp
button_state_h += $(types_h)

input_h := $(input)/input.hpp
input_h += $(input)/keyboard_input.hpp
input_h += $(input)/mouse_input.hpp
input_h += $(input)/controller_input.hpp
input_h += $(button_state_h)

input_state_h := $(input)/input_state.hpp
input_state_h += $(input_h)

sdl_include_h := $(sdl)/sdl_include.hpp
sdl_include_h += $(input_h)


main_c         := $(sdl)/sdl_main.cpp
main_o         := $(build)/main.o
object_files   := $(main_o)

main_dep := $(sdl_include_h)
main_dep += $(stopwatch_h)


sdl_input_c := $(sdl)/sdl_input.cpp
sdl_input_o := $(build)/sdl_input.o
object_files += $(sdl_input_o)

sdl_input_dep := $(sdl_include_h)
sdl_input_dep += $(input_state_h)



$(main_o): $(main_c) $(main_dep)
	@echo "main"
	$(GPP) -o $@ -c $< $(SDL2)


$(sdl_input_o): $(sdl_input_c) $(sdl_input_dep)
	@echo "sdl_input"
	$(GPP) -o $@ -c $< $(SDL2)


$(program_exe): $(object_files)
	@echo "\n $(exe)"
	$(GPP) -o $@ $+ $(ALL_LFLAGS)



build: $(program_exe)


run: build
	$(program_exe)
	@echo "\n"


clean:
	rm -rfv $(build)/*

setup:
	mkdir -p $(build)