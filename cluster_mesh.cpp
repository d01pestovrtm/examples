/**
 * @file cluster_mesh.cpp
 * @brief Cluster the mesh without considering its geometry.
 * @author Dmitrij Pesztov
 */
 
#include <opencv2/viz.hpp>
#include <iostream>
#include <vector>
#include <string>

/**
 * @function help
 * @brief Display instructions to use this tutorial program
 */
static void help()
{
    std::cout
    << "--------------------------------------------------------------------------" << std::endl
    << " This program demonstrates how to cluster a mesh without\n" 
    << "considering its geometry.\n"
    << " The mesh is created from a user-specified file in PLY or OBJ format.\n"
    << " The files with prepared models could be found in internet\n"
    << "Usage:"                                                                     << std::endl
    << "./cluster_mesh [ FILENAME ]"                                             << std::endl
    << std::endl;
} 

 
/**
 * @function main
 */
int main(int argn, char **argv) 
{
	help();
	
	if (argn < 2)
	{
		std::cout << "Missing filename" << std::endl;
		return 1;
	}
	
	/// Create a mesh from file
	std::string filename = argv[1];
	int type = (filename.back() == 'y') ? cv::viz::Mesh::LOAD_PLY : cv::viz::Mesh::LOAD_OBJ;
	cv::viz::Mesh mesh = cv::viz::Mesh::load(filename, type);
	cv::Mat &cloud = mesh.cloud; 
	
	/// Set up clusterization
        cv::Mat labels;
        std::vector <cv::Point3f> centers;
        const int clusterCount = 7;
        double compactness = cv::kmeans(cloud, clusterCount, labels,
            cv::TermCriteria( cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1.0),
               5, cv::KMEANS_PP_CENTERS, centers);
	
	/// OpenCV uses BGR, not RGB
        cv::Vec3b colors[] =
        {
                cv::Vec3b(0, 0, 255), //red
                cv::Vec3b(0, 255, 0), //green
                cv::Vec3b(255, 0, 0), //blue
                cv::Vec3b(0, 255, 255), //yellow
                cv::Vec3b(128, 0, 128), //purple
                cv::Vec3b(0, 165, 255), //orange
                cv::Vec3b(203, 192, 255) //pink
        };
        
        /// Color the points based on their assigned clusters
	cv::Mat point_colors(1, cloud.cols, CV_8UC3);
	for (int i = 0; i < point_colors.cols; ++i) {
		int cluster_idx = labels.at<int>(i);
		point_colors.at<cv::Vec3b>(i) = colors[cluster_idx];
	}
	mesh.colors = point_colors;
	
	/// Create a window
	cv::viz::Viz3d myWindow("ClusterizedMesh");
	
	/// Create a widget for our mesh
	cv::viz::WMesh widget(mesh);
	myWindow.showWidget("mesh widget", widget);
	
	
	/// Mark the centers of clusters
        std::vector <cv::viz::WSphere> clusters;
        for (int i = 0; i < centers.size(); ++i) {
                clusters.push_back( cv::viz::WSphere(centers[i], 0.25, 10, cv::viz::Color::white()) );
                myWindow.showWidget("cluster " + std::to_string(i), clusters[i]);
        }
        
	myWindow.spin();
	return 0;
}
