file(GLOB SOURCES *.c *.cpp "src" "src/base" "src/Fonts" "src/freetype" "src/sfnt" "src/smooth" "src/truetype" "src/utility")
file(GLOB HEADERS "src" "src/base" "src/Fonts" "src/Fonts/Custom" "src/Fonts/GFXFF" "src/freetype" "src/freetype/config" "src/freetype/internal" "src/sfnt" "src/smooth" "src/truetype" "src/utility")
set(requires "driver" "arduino-esp32")

idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS . ${HEADERS}
                    REQUIRES ${requires}
)