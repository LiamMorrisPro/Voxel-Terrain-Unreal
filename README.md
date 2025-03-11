This is a quick project i worked on to get familiarized with coding C++ in unreal engine.

It's a simple voxel terrain generator that generates a noise texture, interprets it as a heightmap, and uses that heightmap to calculate the block data. Then it generates the faces of the blocks using a greedy meshing algorithm.
![VoxShowcase](https://github.com/user-attachments/assets/4c3f07fe-9a3d-4b09-80df-14aef836d630)

Overall it is pretty efficient, though there are some points where it could be improved. The meshing doesn't work properly in-between chunks so there are a bunch of faces along the wall of the chunks out of the players view. The greedy meshing algorithm also seems to comprimise the chunks ability to properly do occlusion culling.
