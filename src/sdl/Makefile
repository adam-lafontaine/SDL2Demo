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

qsprintf := $(util)/qsprintf

types_h     := $(util)/types.hpp
stopwatch_h := $(util)/stopwatch.hpp
qsprintf_h  := $(qsprintf)/qsprintf.hpp

memory_buffer_h := $(util)/memory_buffer.hpp
memory_buffer_h += $(types_h)

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
image_h += $(memory_buffer_h)

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
main_dep += $(app_h)

#************


#*** util ***

util_c       := $(util)/util.cpp
util_o       := $(build)/util.o
object_files += $(util_o)

util_dep := $(qsprintf_h)
util_dep += $(qsprintf)/qsprintf.cpp

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
image_dep += $(image)/stb_image.h
image_dep += $(image)/stb_include.hpp

#************


#*** app ***

app_c        := $(app)/app.cpp
app_o        := $(build)/app.o
object_files += $(app_o)

app_dep := $(app_h)
app_dep += $(image_h)

#************


$(main_o): $(main_c) $(main_dep)
	@echo "\n  main"
	$(GPP) -o $@ -c $< $(SDL2)


$(util_o): $(util_c) $(util_dep)
	@echo "\n  util"
	$(GPP) -o $@ -c $< $(NO_FLAGS)


$(sdl_input_o): $(sdl_input_c) $(sdl_input_dep)
	@echo "\n  sdl_input"
	$(GPP) -o $@ -c $< $(SDL2)


$(app_o): $(app_c) $(app_dep)
	@echo "\n  app"
	$(GPP) -o $@ -c $< $(TBB)


$(image_o): $(image_c) $(image_dep)
	@echo "\n  image"
	$(GPP) -o $@ -c $< $(NO_FLAGS)


$(program_exe): $(object_files)
	@echo "\n  program_exe"
	$(GPP) -o $@ $+ $(ALL_LFLAGS)



build: $(program_exe)


run: build
	$(program_exe)
	@echo "\n"


clean:
	rm -rfv $(build)/*

setup:
	mkdir -p $(build)