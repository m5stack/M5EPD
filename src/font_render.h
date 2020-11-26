// SPDX-License-Identifier: MIT

#pragma once

#include "esp_err.h"

#include "ft2build.h"
#include FT_FREETYPE_H

typedef FT_Long font_data_size_t;
typedef FT_UInt font_size_t;
typedef FT_Byte font_data_t;


typedef struct glyph_cache_record {
	uint32_t utf_code;
	uint16_t priority;
	font_size_t bitmap_width;
	font_size_t bitmap_height;
	FT_Glyph_Metrics metrics;
	int bitmap_left;
	int bitmap_top;
	int advance;
} glyph_cache_record_t;

typedef struct font_face {
	FT_Face ft_face;
	font_size_t pixel_size;
} font_face_t;


typedef struct font_render {
	font_face_t *font_face;
	font_size_t max_pixel_width;
	font_size_t max_pixel_height;
	font_size_t origin;
	font_size_t bitmap_width;
	font_size_t bitmap_height;
	font_size_t pixel_size;
	uint16_t cache_size;
	size_t bytes_per_glyph;
	uint8_t *glyph_cache;
	glyph_cache_record_t *glyph_cache_records;
	FT_Glyph_Metrics metrics;
	int bitmap_left;
	int bitmap_top;
	int advance;
	uint8_t *bitmap;
	uint16_t current_priority;
} font_render_t;

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t font_face_init_fs(font_face_t *face, const char *path);
esp_err_t font_face_init(font_face_t *face, const font_data_t *data, font_data_size_t size);
void font_face_destroy(font_face_t *face);
esp_err_t font_face_set_pixel_size(font_face_t *face, font_size_t pixel_size);

esp_err_t font_render_init(font_render_t *render, font_face_t *face, font_size_t pixel_size, uint16_t cache_size);
void font_render_destroy(font_render_t *render);
esp_err_t font_load_glyph_metrics(font_render_t *render, uint32_t utf_code);
esp_err_t font_render_glyph(font_render_t *render, uint32_t utf_code);


#ifdef __cplusplus
}
#endif
