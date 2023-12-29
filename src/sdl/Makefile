GPP := g++-11 -std=c++17

#GPP += -O3
#GPP += -DNDEBUG

# apt-get install libsdl2-dev
SDL2 := `sdl2-config --cflags --libs`
TBB := -ltbb
NO_FLAGS := 

ALL_LFLAGS := $(SDL2) $(TBB)

root  := ../..
build := $(root)/build/sdl
src   := $(root)/src
util  := $(src)/util
input := $(src)/input
sdl   := $(src)/sdl
image := $(src)/image
app   := $(src)/app

exe := sdl_app

program_exe := $(build)/$(exe)


#*** util ***

stopwatch_h := $(util)/stopwatch.hpp
types_h := $(util)/types.hpp

#***********


#*** input ***

button_state_h := $(input)/button_state.hpp
button_state_h += $(types_h)

input_h := $(input)/input.hpp
input_h += $(input)/keyboard_input.hpp
input_h += $(input)/mouse_input.hpp
input_h += $(input)/controller_input.hpp
input_h += $(button_state_h)

input_state_h := $(input)/input_state.hpp
input_state_h += $(input_h)

#**************


#*** sdl ***

sdl_include_h := $(sdl)/sdl_include.hpp
sdl_include_h += $(input_h)

#**********


#*** image ***

image_h := $(image)/image.hpp
image_h += $(types_h)

#*************


#*** app ***

app_h := $(app)/app.hpp
app_h += $(input_h)

#************


#*** main ***

main_c         := $(sdl)/sdl_main.cpp
main_o         := $(build)/main.o
object_files   := $(main_o)

main_dep := $(sdl_include_h)
main_dep += $(stopwatch_h)

#************


#*** sdl ***

sdl_input_c  := $(sdl)/sdl_input.cpp
sdl_input_o  := $(build)/sdl_input.o
object_files += $(sdl_input_o)

sdl_input_dep := $(sdl_include_h)
sdl_input_dep += $(input_state_h)

#************


#*** image ***

image_c      := $(image)/image.cpp
image_o      := $(build)/image.o
object_files += $(image_o)

image_dep := $(image_h)

#************


#*** app ***

app_c        := $(app)/app.cpp
app_o        := $(build)/app.
object_files += $(app_o)

app_dep := $(app_h)

#************


$(main_o): $(main_c) $(main_dep)
	@echo "main"
	$(GPP) -o $@ -c $< $(SDL2)


$(sdl_input_o): $(sdl_input_c) $(sdl_input_dep)
	@echo "sdl_input"
	$(GPP) -o $@ -c $< $(SDL2)


$(app_o): $(app_c) $(app_dep)
	@echo "app"
	$(GPP) -o $@ -c $< $(TBB)


$(image_o): $(image_c) $(image_dep)
	@echo "image"
	$(GPP) -o $@ -c $< $(NO_FLAGS)


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