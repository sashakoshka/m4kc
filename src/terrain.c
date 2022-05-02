#include <stdio.h>
#include "gameloop.h"
#include "terrain.h"
#include "blocks.h"
#include "data.h"

static void chunkFilePath (World *, char *, int, int, int);
static int  Chunk_save    (World *, Chunk *);

/* World_sort
 * Sorts all chunks in a world by hash
 */
void World_sort (World* world) {
        int i, j;
        Chunk temp;
        for (i = 0; i < CHUNKARR_SIZE; i++)
        for (j = 0; j < (CHUNKARR_SIZE - 1 - i); j++)
        if (
                world->chunk[j].coordHash < world->chunk[j + 1].coordHash
        ) {
                temp = world->chunk[j];
                world->chunk[j] = world->chunk[j + 1];
                world->chunk[j + 1] = temp;
        }
}

/* World_save
 * Saves all loaded chunks in a world to disk. Returns 0 on success, non-zero on
 * failure.
 */
int World_save (World *world) {
        if (data_ensureDirectoryExists(world->path)) { return 1; }

        for (int index = 0; index < CHUNKARR_SIZE; index ++) {
                Chunk *chunk = &world->chunk[index];

                if (chunk->loaded) {
                        int err = Chunk_save(world, chunk);
                        if (err) { return err; }
                }

        }

        char metadataPath[PATH_MAX];
        data_getWorldMetaPath(metadataPath, world->path);
        
        // FILE *metadata = fopen()
        return 0;
}

/* World_load
 * Loads the specified save into a world struct. This does not load any chunks,
 * as that will be handled automatically later.
 */
int World_load (World *world, const char *name) {
        int err = data_getWorldPath(world->path, name);
        if (err) { return err; }

        return 0;
}

/* Chunk_save
 * Saves a chunk to disk. Returns 0 on success, non-zero on failure.
 */
static int Chunk_save (World *world, Chunk *chunk) {
        char path[PATH_MAX];
        chunkFilePath (
                world, path,
                chunk->center.x - 32,
                chunk->center.y - 32,
                chunk->center.z - 32);
        
        FILE *file = fopen(path, "wb");
        if (file == NULL) { return 2; }
        
        fwrite (
                chunk->blocks,
                sizeof(Block),
                CHUNK_DATA_SIZE,
                file);
        fclose(file);

        return 0;
}

/* chunkHash
 * Produces a chunk hash from the specified coordinates. Expects coordinates to
 * be by chunk, not by block.
 */
u_int32_t chunkHash (int x, int y, int z) {
        // Modulo-like operation by bitmasking
        x &= 0b1111111111;
        y &= 0b1111111111;
        z &= 0b1111111111;

        // Move these into their correct "slots"
        y <<= 10;
        z <<= 20;

        // Flatten them using binary or.
        return (x | y | z) + 1;
}

/* chunkFilePath
 * Returns the file name of a chunk.
 */
static void chunkFilePath (World *world, char *path, int x, int y, int z) {
        snprintf (
                path, PATH_MAX,
                "%s/%08x-%08x-%08x.ch",
                world->path,
                x >> 6, y >> 6, z >> 6);
}

/* chunkLookup
 * Takes in a world pointer, and returns a pointer to the chunk
 * at the specific x y and z coordinates. If the chunk is the
 * same as last time, it does not do another lookup, meaning this
 * function can be called very frequently.
 */
Chunk *chunkLookup (World *world, int x, int y, int z) {
        static Chunk *chunk;
        // Rather unlikely position. Not a coord because integers are
        // faster
        static IntCoords ago = {100000000, 100000000, 100000000};
        // Divide by 64
        x >>= 6;
        y >>= 6;
        z >>= 6;
        if (
                ago.x != x ||
                ago.y != y ||
                ago.z != z
        ) {
                ago.x = x;
                ago.y = y;
                ago.z = z;

                u_int32_t hash = chunkHash(x, y, z);

                // Look up chunk using a binary search
                int first, middle, last;

                first  = 0,
                last   = CHUNKARR_SIZE - 1,
                middle = (CHUNKARR_SIZE - 1) / 2;

                while (first <= last) {
                        if (world->chunk[middle].coordHash > hash) {
                                first = middle + 1;
                        } else if (world->chunk[middle].coordHash == hash) {
                                chunk = &world->chunk[middle];
                                return chunk;
                        } else {
                                last = middle - 1;
                        }
                        middle = (first + last) / 2;
                }
                chunk = NULL;
        }
        return chunk;
}

/* World_setBlock
 * Takes in a world, xyz coordinates, and a block id.
 * Returns true if the block could be set, otherwise returns
 * false. Eventually will add block to a stack of set requests if
 * the chunk is not loaded, and will set the block when the chunk
 * loads. If force is true, blocks other than air will be set.
 */
int World_setBlock (
        World *world,
        int x, int y, int z,
        Block block,
        int force
) {
        static int   b;
        static Chunk *chunk;
        b = World_getBlock(world, x, y, z) == BLOCK_AIR;

        if (force || b) { // If the block was air or we don't care
                chunk = chunkLookup(world, x, y, z);

                // If chunk does not have an allocated block array, exit
                if (chunk == NULL || !chunk->loaded) return -1;

                chunk->blocks [
                        nmod(x, CHUNK_SIZE) +
                        (nmod(y, CHUNK_SIZE) * CHUNK_SIZE) +
                        (nmod(z, CHUNK_SIZE) * CHUNK_SIZE * CHUNK_SIZE)
                ] = block;
                return b;
        } else {
                return 0;
        }
}

/* World_getBlock
 * Takes in a world, xyz coordinates, and outputs the block
 * id there. Returns -1 if chunk is not loaded
 */
Block World_getBlock (
        World *world,
        int x, int y, int z
) {
        static Chunk *chunk;
        chunk = chunkLookup(world, x, y, z);

        // If chunk does not have an allocated block array, exit
        if(chunk == NULL || !chunk->loaded) return -1;

        return chunk->blocks [
                nmod(x, CHUNK_SIZE) +
                (nmod(y, CHUNK_SIZE) * CHUNK_SIZE) +
                (nmod(z, CHUNK_SIZE) * CHUNK_SIZE * CHUNK_SIZE)
        ];
}

/* ch_setBlock
 * Takes in a blocks array, xyz coordinates, and a block id.
 * Sets the block. For usage in terrain generation. Returns false
 * if the block was previously air. If force is false, only air will be set.
 */
int ch_setBlock (
        Block *blocks,
        int x, int y, int z,
        Block block,
        int force
) {
        if (x < 0           || y < 0           || z < 0          ) { return 0; }
        if (x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE) { return 0; }
        int notAir = blocks [
                x +
                (y * CHUNK_SIZE) +
                (z * CHUNK_SIZE * CHUNK_SIZE)
        ] != BLOCK_AIR;
        if (!force && notAir) { return notAir; }
        blocks[x + (y * CHUNK_SIZE) + (z * CHUNK_SIZE * CHUNK_SIZE)] = block;
        return notAir;
}

/* ch_getBlock
 * Takes in a blocks array, xyz coordinates, and returns the block id at those
 * coordinates. For usage in terrain generation.
 */
Block ch_getBlock (
        Block *blocks,
        int x, int y, int z
) {
        if (x < 0           || y < 0           || z < 0          ) {
                return BLOCK_AIR;
        }
        if (x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE) {
                return BLOCK_AIR;
        }
        return blocks[x + (y * CHUNK_SIZE) + (z * CHUNK_SIZE * CHUNK_SIZE)];
}

/* ch_setCube
 * Takes in a block array, xyz coordinates, dimensions, and fills
 * in a cube with the specified block. If force is true, blocks
 * other than air will be filled. If no blocks were previously
 * air, returns false.
 */
int ch_setCube (
        Block *blocks,
        int x, int y, int z,
        int w, int h, int l,
        Block block,
        int force
) {
        static int xx, yy, zz, blockPlaced;
        x --; y --; z --; blockPlaced = 0;
        for (xx = w + x; xx > x; xx--)
        for (yy = h + y; yy > y; yy--)
        for (zz = l + z; zz > z; zz--) {
                blockPlaced |= ch_setBlock(blocks, xx, yy, zz, block, force);
        }
        return blockPlaced;
}

/* genStructure
 * Takes in a block array, xyz coordinates, and generates the
 * specified structure.
 */
void genStructure (
        Block *blocks,
        int x, int y, int z,
        int type
) {
        /* Structure ideas
         * 
         * - obelisks
         * - villages
         * - giant trees with villages in them
         * - brick houses
         * - statues
         * - mineshafts
         * - bridges
         * - fortresses
         */
        switch (type) {
        case 0: // tree
                for (int trunk = randm(2) + 4; trunk > 0; trunk --) {
                        ch_setBlock(blocks, x, y --, z, BLOCK_WOOD, 1);
                }
                ch_setCube (blocks, x - 2, y + 1, z - 2, 5, 2, 5,
                        BLOCK_LEAVES, 0);
                ch_setCube (blocks, x - 1, y - 1, z - 1, 3, 2, 3,
                        BLOCK_LEAVES, 0);
                break;

        case 1: // pyramid
                y -= 5 + randm(2);
                int cubePlaced = 1;
                for (int step = 1; cubePlaced > 0 && step < 64; step += 2) {
                        cubePlaced &= ch_setCube (
                                blocks,
                                x - step / 2,
                                y ++,
                                z - step / 2,
                                step, 1, step,
                                BLOCK_BRICKS, 1
                        );
                }
                break;
        }
}

/* genChunk
 * Takes in a seed and a chunk array. Chunk is 64x64x64 blocks. Fills the chunk
 * array with generated terrain. If force is set to true, a chunk at the same
 * coordinates will be overwritten. If the chunk exists on disk, nothing is
 * generated and the file is loaded instead. Returns 1 if the chunk was
 * generated or loaded, and 0 if it wasn't.
 */
int genChunk (
        World *world,
        unsigned int seed,
        int xOffset,
        int yOffset,
        int zOffset,
        int type,
        int force,
        Coords coords
) {
        xOffset = (xOffset / CHUNK_SIZE) * CHUNK_SIZE;
        yOffset = (yOffset / CHUNK_SIZE) * CHUNK_SIZE;
        zOffset = (zOffset / CHUNK_SIZE) * CHUNK_SIZE;
        // To make sure structure generation accross chunks is
        // different, but predictable
        srand(seed * (xOffset * yOffset * zOffset + 1));
        int i, distMax, distMaxI;
        static int count = 0;

        Chunk *chunk = chunkLookup(world, xOffset, yOffset, zOffset);

        // Only generate if that chunk hasn't been generated yet.
        i = 0;
        if (chunk == NULL) {
                // See if there is an empty slot
                for (; i < CHUNKARR_SIZE && world->chunk[i].loaded; i ++);

                // Pick out the oldest chunk (loaded) and overrwrite it.
                if (i == CHUNKARR_SIZE) {
                        distMax  = 0;
                        distMaxI = 0;
                        for (i = 0; i < CHUNKARR_SIZE; i ++) {
                                int dist = dist3d (
                                        coords.x - world->chunk[i].center.x,
                                        coords.y - world->chunk[i].center.y,
                                        coords.z - world->chunk[i].center.z,
                                        2, 2, 2);
                                if (dist > distMax) {
                                        distMax  = dist;
                                        distMaxI = i;
                                }
                        }
                        i = distMaxI;
                }
                chunk = &world->chunk[i];
        } else if (!force) {
                return 0;
        }
  
        // If there is no array, allocate one.
        if (chunk->loaded) {
                int err = Chunk_save(world, chunk);
                if (err) { gameLoop_error("Could not save/unload chunk"); }
        } else {
                chunk->blocks = (Block *)calloc(CHUNK_DATA_SIZE, sizeof(Block));
        }

        if (chunk->blocks == NULL) {
                printf("genChunk: memory allocation fail\n");
                return 0;
        }

        Block *blocks = chunk->blocks;

        // If the chunk exists on disk, load it and halt the function
        char path[PATH_MAX];
        chunkFilePath(world, path, xOffset, yOffset, zOffset);
        if (data_fileExists(path)) {
                FILE *file = fopen(path, "rb");
                if (file == NULL) { return 2; }
                
                fread (
                        chunk->blocks,
                        sizeof(Block),
                        CHUNK_DATA_SIZE,
                        file);
                fclose(file);
                return 1;
        }

        for (int i = 0; i < CHUNK_DATA_SIZE; i++) {
                blocks[i] = 0;
        }

        // Generate a hash
        int hashX, hashY, hashZ;

        hashX = xOffset >> 6;
        hashY = yOffset >> 6;
        hashZ = zOffset >> 6;

        hashX &= 0b1111111111;
        hashY &= 0b1111111111;
        hashZ &= 0b1111111111;

        hashY <<= 10;
        hashZ <<= 20;

        chunk->coordHash = hashX | hashY | hashZ;
        chunk->coordHash ++;

        chunk->center.x = xOffset + 32;
        chunk->center.y = yOffset + 32;
        chunk->center.z = zOffset + 32;

        // What we have here won't cause a segfault, so it is safe to
        // mark the chunk as loaded and set its stamp.
        chunk->loaded = ++ count;

        // printf (
                // "chunk hash: %#016x\tx: %i\ty: %i\tz: %i\t"
                // "cx: %i\tcy: %i\tcz: %i\t"
                // "stamp: %i\taddr: %p \tgenerated\n",
                // chunk->coordHash,
                // xOffset, yOffset, zOffset,
                // chunk->center.x,
                // chunk->center.y,
                // chunk->center.z,
                // chunk->loaded, chunk
        // );
        
        switch (type) {
        case -1:
                // Test terrain
                ch_genDev(blocks, xOffset, yOffset, zOffset);
                break;
        case 0:
                // Classic terrain
                ch_genClassic(blocks, yOffset);
                break;
        case 1:
                // New terrain
                ch_genNew(blocks, seed, xOffset, yOffset, zOffset);
                break;
        case 2:
                // Flat stone
                ch_genStone(blocks, yOffset);
                break;
        case 3:
                // Flat grass
                ch_genFlat(blocks, yOffset);
                break;
        case 4:
                // Water world
                ch_genWater(blocks, yOffset);
                break;
        }

        // Sort all chunks
        World_sort(world);
        return 1;
}

void ch_genClassic (Block *blocks, int yOffset) {
        for (int x = 0; x < CHUNK_SIZE; x ++)
        for (int y = 0; y < CHUNK_SIZE; y ++)
        for (int z = 0; z < CHUNK_SIZE; z ++)
        if (y + yOffset > 32) {
                Block block = randm(2) == 0 ? randm(9) : 0;
                if (
                        block == BLOCK_SAND ||
                        block == BLOCK_GRAVEL
                ) { block = BLOCK_DIRT; }
                ch_setBlock(blocks, x, y, z, block, 1);
        }
}

void ch_genNew (
        Block *blocks,
        unsigned int seed,
        int xOffset,
        int yOffset,
        int zOffset
) {
        // Generate heightmap
        int heightmap[CHUNK_SIZE][CHUNK_SIZE];
        for (int x = 0; x < CHUNK_SIZE; x ++)
        for (int z = 0; z < CHUNK_SIZE; z ++) {
                heightmap[x][z] =
                        perlin2d ( // Base noise
                                seed,
                                x + xOffset + 0xFFFFFF,
                                z + zOffset + 0xFFFFFF,
                                0.0625
                        ) * 16 +
                        perlin2d ( // Detail noise
                                seed,
                                x + xOffset + 0xFFFFFF,
                                z + zOffset + 0xFFFFFF,
                                0.0078125
                        ) * 64;
        }

        // Make terrain from heightmap
        for (int x = 0; x < CHUNK_SIZE; x ++)
        for (int y = 0; y < CHUNK_SIZE; y ++)
        for (int z = 0; z < CHUNK_SIZE; z ++) {
                if (y + yOffset > heightmap[x][z] + 4) {
                        ch_setBlock(blocks, x, y, z, BLOCK_STONE, 1);
                } else if (y + yOffset > heightmap[x][z]) {
                        ch_setBlock(blocks, x, y, z, BLOCK_DIRT, 1);
                } else if (y + yOffset == heightmap[x][z]) {
                        if (y + yOffset < 44) {
                                ch_setBlock(blocks, x, y, z, BLOCK_GRASS, 1);
                        } else {
                                ch_setBlock(blocks, x, y, z, BLOCK_SAND, 1);
                        }
                } else if (y + yOffset < 45) {
                        ch_setBlock(blocks, x, y, z, BLOCK_AIR, 1);
                } else {
                        ch_setBlock(blocks, x, y, z, BLOCK_WATER, 1);
                }
        }

        // Generate caves
        for (int x = 0; x < CHUNK_SIZE; x ++)
        for (int z = 0; z < CHUNK_SIZE; z ++) {
                float noisePoint = perlin2d (
                        seed + yOffset,
                        x + xOffset + 0xFFFFFF,
                        z + zOffset + 0xFFFFFF,
                        0.0625
                );

                if (noisePoint < 0.47 || noisePoint > 0.53) { continue; }

                int elevation = perlin2d (
                        seed + 2 + yOffset,
                        x + xOffset + 0xFFFFFF,
                        z + zOffset + 0xFFFFFF,
                        0.0625
                ) * 8;

                int height = perlin2d (
                        seed + 3 + yOffset,
                        x + xOffset + 0xFFFFFF,
                        z + zOffset + 0xFFFFFF,
                        0.0625
                ) * 4 + 2 - (randm(1) > 0);

                int lowPoint  = 64 - elevation;
                int highPoint = 64 - elevation - height;

                for (int y = highPoint; y < lowPoint; y ++) {
                        ch_setBlock(blocks, x, y, z, BLOCK_AIR, 1);
                }

                // Don't have bare dirt on the bottom
                if (ch_getBlock(blocks, x, lowPoint, z) == 2) {
                        // What block we place down depends on the block above
                        if (
                                ch_getBlock(blocks, x, highPoint - 1, z) ==
                                        BLOCK_AIR
                        ) {
                                ch_setBlock (blocks, x, lowPoint, z,
                                        BLOCK_GRASS, 1);
                        } else {
                                ch_setBlock (blocks, x, lowPoint, z,
                                        BLOCK_GRAVEL, 1);
                        }
                }
        }

        // Generate pyramids
        srand(seed * (xOffset * yOffset * zOffset + 2));
        for (int i = randm(2); i > 0; i --) {
                int x = randm(64);
                int z = randm(64);

                genStructure (
                        blocks,
                        x, heightmap[x][z] + 1, z,
                        1
                );
        }
        
        // Generate trees
        srand(seed * (xOffset * yOffset * zOffset + 3));
        for (int i = randm(16) + 64; i > 0; i --) {
                int x = randm(64);
                int z = randm(64);

                if (ch_getBlock(blocks, x, heightmap[x][z], z) != BLOCK_GRASS) {
                        continue;
                }

                genStructure (
                        blocks,
                        x, heightmap[x][z] - 1, z,
                        0
                );

        }
        
        // Plant tall grass
        srand(seed * (xOffset * yOffset * zOffset + 4));
        for (int x = 0; x < CHUNK_SIZE; x ++)
        for (int z = 0; z < CHUNK_SIZE; z ++)
        if (
                ch_getBlock(blocks, x, heightmap[x][z], z) == BLOCK_GRASS &&
                randm(2) == 0
        ) {
                ch_setBlock (blocks,
                        x, heightmap[x][z] - 1, z,
                        BLOCK_TALL_GRASS, 0);
        }
}

void ch_genStone (Block *blocks, int yOffset) {
        for (int x = 0; x < CHUNK_SIZE; x ++)
        for (int y = 0; y < CHUNK_SIZE; y ++)
        for (int z = 0; z < CHUNK_SIZE; z ++)
        if (y + yOffset > 32) {
                ch_setBlock(blocks, x, y, z, BLOCK_STONE, 1);
        } else {
                ch_setBlock(blocks, x, y, z, BLOCK_AIR, 1);
        }
}

void ch_genFlat (Block *blocks, int yOffset) {
        for (int x = 0; x < CHUNK_SIZE; x ++)
        for (int z = 0; z < CHUNK_SIZE; z ++)
        for (int y = 0; y < CHUNK_SIZE; y ++) {
                if (y + yOffset <  32) {
                        ch_setBlock(blocks, x, y, z, BLOCK_AIR, 1);
                }
                if (y + yOffset == 32) {
                        ch_setBlock(blocks, x, y, z, BLOCK_GRASS, 1);
                }
                if (y + yOffset >  32) {
                        ch_setBlock(blocks, x, y, z, BLOCK_DIRT, 1);
                }
        }
}

void ch_genWater (Block *blocks, int yOffset) {
        for (int x = 0; x < CHUNK_SIZE; x ++)
        for (int y = 0; y < CHUNK_SIZE; y ++)
        for (int z = 0; z < CHUNK_SIZE; z ++)
        if (y + yOffset > 64) {
                ch_setBlock(blocks, x, y, z, BLOCK_SAND, 1);
        } else if (y + yOffset > 32) {
                ch_setBlock(blocks, x, y, z, BLOCK_WATER, 1);
        } else {
                ch_setBlock(blocks, x, y, z, BLOCK_AIR, 1);
        }
}

void ch_genDev (Block *blocks, int xOffset, int yOffset, int zOffset) {
        (void)(xOffset);
        (void)(yOffset);
        (void)(zOffset);

        if (yOffset != 0) { return; }

        for (int x = 0; x < CHUNK_SIZE; x ++)
        for (int z = 0; z < CHUNK_SIZE; z ++) {
                if (randm(4) == 0) {
                        ch_setBlock(blocks, x, 3, z, BLOCK_TALL_GRASS, 1);
                }
                ch_setBlock(blocks, x, 4, z, BLOCK_DIRT, 1);
        }
}
