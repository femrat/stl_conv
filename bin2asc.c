#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>

#define STL_HEADER_LEN 80

typedef struct {
	uint8_t  info[STL_HEADER_LEN];
	uint32_t count;
} stl_header_t;

#pragma pack(push)
#pragma pack(1)
typedef struct {
	float    normal[3];
	float    v1[3];
	float    v2[3];
	float    v3[3];
	uint16_t attr;
} stl_slice_t;
#pragma pack(pop)

size_t Fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	if ((size = fread(ptr, size, nmemb, stream)) != nmemb) {
		printf("Error while reading\n");
		exit(1);
	}
	return size;
}

size_t Fprintf(FILE *stream, const char *format, ...) {
	va_list argptr;
	size_t size;
	va_start(argptr, format);
	if ((size = vfprintf(stream, format, argptr)) < 0) {
		printf("Error while writing\n");
		exit(1);
	}
	va_end(argptr);
	return size;
}

int main(int argc, char *argv[]) {
	
	if (argc != 3 && argc != 4) {
		printf("Convert from binary STL to ascii STL.\n"
		       "Usage: %s binary_file ascii_file [skip_slice_count]\n"
			   "Written by Femrat <femrat@gmail.com>\n", 
			   argv[0]);
		return 0;
	}

	uint32_t skip;
	if (argc == 4) {
		if (sscanf(argv[3], "%d", &skip) != 1) {
			printf("Invalid argument: %s\n", argv[3]);
			exit(1);
		}
	} else {
		skip = 0;
	}

	FILE *in  = fopen(argv[1], "r");
	FILE *out = fopen(argv[2], "w");

	if (in == NULL || out == NULL) {
		printf("Error while opening file\n");
		return 0;
	}

	uint32_t i;
	stl_header_t header;
	stl_slice_t  slice;

	Fread(&header, sizeof(header), 1, in);

	printf("STL title:\n"
		   "========================\n");
	for (i = 0; i < STL_HEADER_LEN; ++i) {
		putchar(header.info[i]);
	}
	printf("\n========================\n\n");

	printf("Total: %d slices\n\n", header.count);
	if (skip) {
		printf("Skipped %d slices\n\n", skip);
	}

	Fprintf(out, "solid %s\n", argv[2]);

	for (i = 0; i < header.count; ++i) {
		Fread(&slice, sizeof(slice), 1, in);
		if (skip == 0) {
			Fprintf(out, "facet normal %f %f %f\n"
						 "outer loop\n"
						 "vertex %f %f %f\n"
						 "vertex %f %f %f\n"
						 "vertex %f %f %f\n"
						 "endloop\n"
						 "endfacet\n",
					slice.normal[0], slice.normal[1], slice.normal[2],
					slice.v1[0], slice.v1[1], slice.v1[2],
					slice.v2[0], slice.v2[1], slice.v2[2],
					slice.v3[0], slice.v3[1], slice.v3[2]);
		} else {
			--skip;
		}
	}

	Fprintf(out, "endsolid %s\n", argv[2]);

	fclose(in);
	fclose(out);

	return 0;
}
