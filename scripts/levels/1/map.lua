world_put(RectangularSectorBuilder.new(1):withDimensions(0, 0, 1, 3):withSouthNeighbour(2, 1, 3, 0, 3))
world_put(RectangularSectorBuilder.new(2):withDimensions(-1, 3, 2, 6):withNorthNeighbour(1, 0, 3, 1, 3):withSouthNeighbour(3, 1, 6, 0, 6))
world_sprite(2, 0, "res/gfx/sprites/lamp.png", 0.5, 4.5)
world_sprite(2, 1, "res/gfx/sprites/metal_table.png", 1.5, 3.5)
world_put(RectangularSectorBuilder.new(3):withDimensions(0, 6, 1, 10):withNorthNeighbour(2, 0, 6, 1, 6):withEastNeighbour(4, 1, 8, 1, 9))
world_sprite(3, 0, "res/gfx/sprites/pipe_with_steam_back.png", 0.5, 9.5)
world_put(RectangularSectorBuilder.new(4):withDimensions(1, 8, 7, 9):withWestNeighbour(3, 1, 9, 1, 8):withSouthNeighbour(5, 4, 9, 3, 9):withNorthNeighbour(12, 4, 8, 5, 8):withEastNeighbour(14, 7, 8, 7, 9))
world_put(RectangularSectorBuilder.new(14):withDimensions(7, 8, 11, 9):withWestNeighbour(4, 7, 9, 7, 8):withNorthNeighbour(15, 8, 8, 9, 8):withSouthNeighbour(17, 8, 9, 7, 9))
world_sprite(14, 0, "res/gfx/sprites/rocks.png", 10.5, 8.5)
world_put(RectangularSectorBuilder.new(5):withDimensions(3, 9, 4, 12):withNorthNeighbour(4, 3, 9, 4, 9):withEastNeighbour(7, 4, 10, 4, 11):withWestNeighbour(6, 3, 12, 3, 10):withSouthNeighbour(8, 4, 12, 3, 12))
world_put(RectangularSectorBuilder.new(6):withDimensions(2, 10, 3, 12):withEastNeighbour(5, 3, 10, 3, 12))
world_put(RectangularSectorBuilder.new(7):withDimensions(4, 10, 5, 11):withWestNeighbour(5, 4, 11, 4, 10))
world_put(RectangularSectorBuilder.new(8):withDimensions(3, 12, 5, 13):withNorthNeighbour(5, 3, 12, 4, 12):withSouthNeighbour(9, 4, 13, 3, 13))
world_put(RectangularSectorBuilder.new(9):withDimensions(2, 13, 4, 14):withNorthNeighbour(8, 3, 13, 4, 13):withSouthNeighbour(10, 4, 14, 3, 14))
world_put(RectangularSectorBuilder.new(10):withDimensions(3, 14, 5, 16):withNorthNeighbour(9, 3, 14, 4, 14):withWestNeighbour(11, 3, 16, 3, 15))
world_put(RectangularSectorBuilder.new(11):withDimensions(2, 15, 3, 16):withEastNeighbour(10, 3, 15, 3, 16))

-- Pokój jakiś
world_put(RectangularSectorBuilder.new(12):withDimensions(4, 7, 5, 8):withSouthNeighbour(4, 5, 8, 4, 8):withNorthNeighbour(13, 4, 7, 5, 7))
world_put(RectangularSectorBuilder.new(13):withDimensions(3, 4, 6, 7):withSouthNeighbour(12, 5, 7, 4, 7))

-- Pokój Oli
world_put(RectangularSectorBuilder.new(15):withDimensions(8, 7, 9, 8):withSouthNeighbour(14, 9, 8, 8, 8):withNorthNeighbour(16, 8, 7, 9, 7))
world_put(RectangularSectorBuilder.new(16):withDimensions(7, 4, 10, 7):withSouthNeighbour(15, 9, 7, 8, 7))
world_sprite(16, 0, "res/gfx/sprites/metal_table.png", 9.5, 4.5)
world_sprite(16, 1, "res/gfx/sprites/ola.png", 9.5, 5.5)

-- Pokój z polybiusem
world_put(RectangularSectorBuilder.new(17):withDimensions(7, 9, 8, 10):withNorthNeighbour(14, 7, 9, 8, 9):withSouthNeighbour(18, 8, 10, 7, 10))
world_put(RectangularSectorBuilder.new(18):withDimensions(6, 10, 9, 13):withNorthNeighbour(17, 7, 10, 8, 10))
