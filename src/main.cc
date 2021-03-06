#include <CGAL/Cartesian.h>
#include <CGAL/Exact_rational.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Arrangement_2.h>
#include <CGAL/Arr_extended_dcel.h>
#include <CGAL/Arr_overlay_2.h>
#include <CGAL/Arr_default_overlay_traits.h>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <strstream>
#include <set>
#include <unordered_set>
#include <chrono>

#include "arrangement.h"
#include "camera.h"
#include "mesh.h"
#include "divide_conquer_construct.h"
#include "plane_param.h"
#include "postprocess.h"

int main (int argc, char** argv)
{
	Mesh mesh;
	Camera camera;
	mesh.LoadFromFile(argv[1]);
	camera.LoadFromFile(argv[2]);
	camera.ApplyExtrinsic(mesh);


	mesh.BoundaryClip(2, 1e-2, 0, false);
	camera.ApplyIntrinsic(mesh);


	mesh.BoundaryClip(0, -0.5, 0, true);
	mesh.BoundaryClip(1, -0.5, 0, true);
	mesh.BoundaryClip(0, 0.5, 1, true);
	mesh.BoundaryClip(1, 0.5, 1, true);



	mesh.ComputeNormals();
	mesh.ComputePlaneParameters();

	Arrangement_2 overlay;


	ConstructArrangement(mesh, 0, mesh.FaceNum() - 1, &overlay);
	//ConstructArrangement(mesh, 0, 1000, &overlay);
	printf("\n");

	//exit(0);
	PostProcess process;	
	process.CollectFaceAndVertices(mesh, overlay, camera.GetAngle());

	// remove 2-degree vertex on edge
	process.RemoveRedundantVertices();

	// merge close vertices
	//process.MergeDuplex(mesh);

	//process.CollectEdges(mesh);

	//recognize faces
	std::cout << argv[3] << std::endl;
	int l = strlen(argv[3]);
	if (l > 3 && argv[3][l - 1] == 'g' && argv[3][l - 2] == 'v' && argv[3][l - 3] == 's')
		process.SaveToSVG(mesh, argv[3]);
	else
		process.SaveToFile(mesh, argv[3]);
}
