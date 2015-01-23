/*
	tmx2lvl (TMX map conversion tool for my awesome C game engine)
	Copyright (C) 2015 Daniel Esteban Nombela <dani@dabuten.co>

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the author be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.

	3. This notice may not be removed or altered from any source distribution.
*/

/**
 *  \file main.c
 *
 *  Main file for tmx2lvl
 */

/* Using Standard lib, Standard I/O, Strings & TMXParser */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Tmx.h>

/**
 *  \brief Lib func to write serialized integers
 *  \param n The integer to write
 *  \param stream The file pointer
 */
void fputb(int n, FILE *stream) {
	fputc((n >> 24) & 0xFF, stream);
	fputc((n >> 16) & 0xFF, stream);
	fputc((n >> 8) & 0xFF, stream);
	fputc(n & 0xFF, stream);
}

/**
 *  \brief The main routine
 *  \param argc The CLI arguments count
 *  \param argv The CLI arguments values
 *
 *  \return The application exit status code
 */
int main(int argc, char *argv[]) {
	/* Verify CLI arguments */
	if(argc != 3) {
		printf("Usage: %s mapfile.tmx mapfile.lvl\n", argv[0]);
		return 1;
	}

	/* Parse the map */
	printf("Parsing the map file \"%s\"...\n", argv[1]);
	Tmx::Map *map = new Tmx::Map();
	map->ParseFile(argv[1]);

	/* Check for errors */
	if(map->HasError()) {
		printf("TMXParser error: %d %s\n", map->GetErrorCode(), map->GetErrorText().c_str());
		return 1;
	}

	/* Verify only one tileset and its name */
	if(map->GetNumTilesets() != 1 || map->GetTileset(0)->GetImage()->GetSource() != "spritesheet.png") {
		printf("Error: Map has to have a single tileset called spritesheet.png\n");
		return 1;
	}

	/* Verify number of tile layers */
	if(map->GetNumLayers() != 2) {
		printf("Error: Map has to have exactly 2 tile layers... Background & Foreground\n");
		return 1;
	}

	/* Verify tile layers names */
	if(map->GetLayer(0)->GetName() != "Background") {
		printf("Error: First tile layer must be named 'Background'\n");
		return 1;
	}
	if(map->GetLayer(1)->GetName() != "Foreground") {
		printf("Error: Second tile layer must be named 'Foreground'\n");
		return 1;
	}

	/* Verify number of object groups */
	if(map->GetNumObjectGroups() != 2) {
		printf("Error: Map has to have exactly 2 object groups... Collision & Spawn\n");
		return 1;
	}

	/* Verify object groups names */
	if(map->GetObjectGroup(0)->GetName() != "Collision") {
		printf("Error: First object group must be named 'Collision'\n");
		return 1;
	}
	if(map->GetObjectGroup(1)->GetName() != "Spawn") {
		printf("Error: Second object group must be named 'Spawn'\n");
		return 1;
	}

	/* Open the output binary file */
	FILE *file = fopen(argv[2], "wb");
	if(file == NULL) {
		printf("ERROR opening file '%s' for writing\n", argv[2]);
		return 1;
	}

	/* Write the map tile width */
	fputb(map->GetTileWidth(), file);

	/* Write the map width */
	fputb(map->GetWidth(), file);

	/* Write the map height */
	fputb(map->GetHeight(), file);

	/* Iterate through the layers */
	for(int i=0; i<map->GetNumLayers(); i++) {
		/* Get the layer */
		const Tmx::Layer *layer = map->GetLayer(i);
		
		/* Iterate through the layer tiles */
		for(int y=0; y<map->GetHeight(); y++) {
			for(int x=0; x<map->GetWidth(); x++) {
				/* Get the tile */
				const Tmx::MapTile tile = layer->GetTile(x, y);

				/* Write the tile id */
				fputc(tile.tilesetId < 0 ? 0xFF : tile.id, file);
			}
		}
	}

	/* Iterate through the object groups */
	for(int i=0; i<map->GetNumObjectGroups(); i++) {
		/* Get the object group */
		const Tmx::ObjectGroup *objectGroup = map->GetObjectGroup(i);

		/* Write the number of objects in the group */
		fputb(objectGroup->GetNumObjects(), file);

		/* Iterate through all the group objects. */
		for(int j=0; j<objectGroup->GetNumObjects(); j++) {
			/* Get the object */
			const Tmx::Object *object = objectGroup->GetObject(j);
			
			/* Write the object name length */
			fputb(object->GetName().length(), file);

			/* Write the object name */
			fputs(object->GetName().c_str(), file);
			
			/* Write the object X */
			fputb(object->GetX(), file);

			/* Write the object Y */
			fputb(object->GetY(), file);

			/* Write the object width */
			fputb(object->GetWidth(), file);

			/* Write the object height */
			fputb(object->GetHeight(), file);
		}
	}

	/* Be polite */
	delete map;
	fclose(file);

	/* Print out a success message */
	printf("\\m/ \\m/ Fuckin' A! \\m/ \\m/\n");
	printf("The map file \"%s\"\n", argv[2]);
	printf("Has been successfully generated without any errors\n");

	return 0;
}
