basic-directx11

- a way to integrate external projects with cmake
https://github.com/premake/premake-core/issues/658

some references:
http://www.codinglabs.net/article_world_view_projection_matrix.aspx
http://www.3dgep.com/understanding-the-view-matrix/
http://www.idav.ucdavis.edu/education/GraphicsNotes/Camera-Transform/Camera-Transform.html

TODO:
Camera lacks the look at vector. We need to get the look at matrix of the camera, then we can do the rotation (and translation?) of that matrix

for view matrix implementation
vitosha/test/irradiance/math/math_graphics.h
vitosha/test/irradiance/win32/window.cpp in render_frame()

Projective geometry and Oriented projective geometry
    - when w > 0 and w < 0, these are 2 different points.
    - when w < 0, then the point is behind the camera, we have to be careful because this point will be projected on the near plane of the frustum, which is wrong

Note:
- when the perspective divide is done on the cpu, the cube seems to be drawn on the whole screen when the camera is close to it, but that problem is gone when the perspective divide is done on the gpu

To create the descriptor structs more easily for various directx api methods, use the these CD3D11 structs:
https://msdn.microsoft.com/en-us/library/windows/desktop/jj151647(v=vs.85).aspx

rendering schema:
- a scene with shapes objects. Have them keep only world positions
- a separate camera class that takes input to update the camera positions
- the camera should be in renderer, so is the scene
- the render function of the renderer should update the camera position every frame
- then the scene is rendered with the camera as parameter to do the view proj transformation to the shape objects
- then render the objects

ID3D11Query
IDXGIFactory

http://aecobjects.com/2014/10/which_format_is_better/

*****************************
generating polyhedrons vertices
https://www.csee.umbc.edu/~squire/reference/polyhedra.shtml

paper about indexing icosahedrons
https://arxiv.org/ftp/cs/papers/0701/0701164.pdf

https://pdfs.semanticscholar.org/3472/9e87a422b448a02fc575779da6f0686aa5ff.pdf?_ga=2.153282046.734739103.1554282333-600310946.1554282333
