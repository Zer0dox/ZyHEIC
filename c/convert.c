#include <stdio.h>
#include <stdlib.h>
#include <libheif/heif.h>
#include <jpeglib.h>

/*
	HEIC to JPEG converter (since this isn't a thing?)
	Seen a post about universities not accepting HEIC files for assignments..
	Now there are easy to use libs :)
	Written proudly in nano on Debian <3
*/

int convert_heic_to_jpeg(const char* heic_path, const char* jpeg_path) {

	// Read the HEIC file
	struct heif_context* ctx = heif_context_alloc();
	struct heif_error err = heif_context_read_from_file(ctx, heic_path, NULL);

	if (err.code != 0) {
		fprintf(stderr, "Error reading HEIC file: %s\n", err.message);
		return -1;
	}

	// Get handle to the primary image
	heif_item_id primary;
	heif_context_get_primary_image_ID(ctx, &primary);
	struct heif_image_handle* handle;
	heif_context_get_image_handle(ctx, primary, &handle);

	// Decode the HEIC image
	struct heif_image* img;
	heif_decode_image(handle, &img, heif_colorspace_RGB, heif_chroma_interleaved_RGB, NULL);

	// Get image data
	int width = heif_image_get_width(img, heif_channel_interleaved);
	int height = heif_image_get_height(img, heif_channel_interleaved);
	int stride;
	const uint8_t* data = heif_image_get_plane_readonly(img, heif_channel_interleaved, &stride);

	// Open JPEG file for writing
	FILE* outfile = fopen(jpeg_path, "wb");
	if (outfile == NULL) {
		fprintf(stderr, "Error opening JPEG file for writing\n");
		return -1;
	}

	// Inititalize JPEG compression
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, outfile);

	// Set JPEG parameters
	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, 90, TRUE);
	jpeg_start_compress(&cinfo, TRUE);

	// Write JPEG data
	while (cinfo.next_scanline < cinfo.image_height) {
		JSAMPROW row_pointer = (JSAMPROW)(data + cinfo.next_scanline * stride);
		jpeg_write_scanlines(&cinfo, &row_pointer, 1);
	}

	// Finish compression and cleanup the mess ;)
	jpeg_finish_compress(&cinfo);
	fclose(outfile);
	jpeg_destroy_compress(&cinfo);
	heif_image_release(img);
	heif_image_handle_release(handle);
	heif_context_free(ctx);

	printf("Conversion successful. File saved as %s\n", jpeg_path);
	return 0;
}

int main(int argc, char *argv[]) {

	if (argc != 3) {
		fprintf(stderr, "Usage: %s input.heic output.jpg\n", argv[0]);
		return -1;
	}

	return convert_heic_to_jpeg(argv[1], argv[2]);

}
