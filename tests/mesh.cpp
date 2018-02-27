#include "test.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include "../src/amesh2dh.hpp"
#include "../src/ameshutils.hpp"

#undef NDEBUG

using namespace acfd;

int test_periodic_map(const std::string mfile, const int bcm, const int axis)
{
	UMesh2dh m;
	m.readMesh(mfile);
	m.compute_topological();
	m.compute_face_data();
	m.compute_periodic_map(bcm,axis);

	// map intfac faces to mesh faces for testing
	m.compute_boundary_maps();

	const int numfaces = 5;
	a_int faces1[] = {8,9,10,11,12};
	a_int faces2[] = {25,24,23,22,21};

	int ierr = 0;

	for(int i = 0; i < numfaces; i++) {
		if(m.gperiodicmap(m.gifbmap(faces1[i])) != m.gifbmap(faces2[i])) {
			ierr = 1;
			std::cerr << "  Face " << faces1[i] << " failed!\n";
		}
		if(m.gperiodicmap(m.gifbmap(faces2[i])) != m.gifbmap(faces1[i])) {
			ierr = 1;
			std::cerr << "  Face " << faces1[i] << " failed!\n";
		}
	}

	return ierr;
}

int test_levelscheduling(const std::string mfile, const std::string levelsfile)
{
	UMesh2dh m;
	m.readMesh(mfile);
	m.compute_topological();
	std::vector<a_int> levels = levelSchedule(m);
	const int nlevels = (int)(levels.size()-1);

	std::ifstream lfile;
	try {
		lfile.open(levelsfile);
	} catch (std::exception& e) {
		throw e;
	}

	int real_nlevels;
	lfile >> real_nlevels;
	std::vector<a_int> real_levels(real_nlevels+1);
	for(int i = 0; i < real_nlevels+1; i++)
		lfile >> real_levels[i];

	lfile.close();
	
	for(int i = 0; i < real_nlevels+1; i++)
		std::cout << real_levels[i] << "  " <<  levels[i] << std::endl;

	TASSERT(real_nlevels == nlevels);
	std::cout << "tested num levels\n";

	for(int i = 0; i < real_nlevels+1; i++)
		TASSERT(real_levels[i] == levels[i]);

	return 0;
}

int test_levelscheduling_internalconsistency(const std::string mfile)
{
	UMesh2dh m;
	m.readMesh(mfile);
	m.compute_topological();
	std::vector<a_int> levels = levelSchedule(m);
	const int nlevels = (int)(levels.size()-1);
	
	for(int ilevel = 0; ilevel < nlevels; ilevel++)
	{
		std::vector<int> marked(m.gnelem(), 0);

		// mark all neighbors of all cells in this level
		// but not the cells in the level themselves
		for(int icell = levels[ilevel]; icell < levels[ilevel+1]; icell++)
		{
			for(int iface = 0; iface < m.gnfael(icell); iface++) {
				if(m.gesuel(icell,iface) < m.gnelem())
					marked[m.gesuel(icell,iface)] = 1;
			}
		}

		// check if the cells in this levels have not been marked
		for(int icell = levels[ilevel]; icell < levels[ilevel+1]; icell++) {
			if(marked[icell]) std::cout << "Problem with " << icell << std::endl;
			TASSERT(!marked[icell]);
		}
	}
	return 0;
}

int main(int argc, char *argv[])
{
	if(argc < 3) {
		std::cout << "Not enough command-line arguments!\n";
		return -2;
	}

	std::string whichtest = argv[1];
	int err = 0;

	if(whichtest == "periodic") {
		err = test_periodic_map(argv[2], 4, 0);
		if(err) std::cerr << " Periodic map test failed!\n";
	}
	else if(whichtest == "levelschedule") {
		if(argc < 4) {
			std::cout << "Not enough command-line arguments!\n";
			return -2;
		}
		err = test_levelscheduling(argv[2], argv[3]);
	}
	else if(whichtest == "levelscheduleInternal") {
		err = test_levelscheduling_internalconsistency(argv[2]);
	}
	else
		throw "Invalid test";

	return err;
}
