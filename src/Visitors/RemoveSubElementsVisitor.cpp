/*
 <Mix-mesher: region type. This program generates a mixed-elements mesh>
 
 Copyright (C) <2013,2017>  <Claudio Lobos>
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/gpl.txt>
 */

#include "RemoveSubElementsVisitor.h"
#include "../Octant.h"

namespace Clobscode
{

    RemoveSubElementsVisitor::RemoveSubElementsVisitor():points(NULL)
    { }

    void RemoveSubElementsVisitor::setPoints(vector<MeshPoint> &points) {
        this->points = &points;
    }
    void RemoveSubElementsVisitor::setTriMesh(TriMesh &mesh) {
        this->mesh = &mesh;
    }
    void RemoveSubElementsVisitor::setFaces(list<unsigned int> &faces){
        this->faces = &faces;
    }
    

    bool RemoveSubElementsVisitor::visit(Octant *o) { //Notes:Visita octante
        vector<vector<unsigned int>> &sub_elements = o->sub_elements;

        list<unsigned int> faces_inter = o->intersected_faces;
        list<vector<unsigned int> > still_in;
        list<vector<unsigned int> > to_review;
        list<vector<unsigned int> >::iterator iter;
        list<vector<unsigned int> >::iterator it;
        IntersectionsVisitor iv; // visitors

        double max_dis = 1.2; //o->getMaxDistance();
        double max_dis_neg = -max_dis;
        for (unsigned int i=0; i<sub_elements.size(); i++) {
            bool onein = false;
            vector<unsigned int> e_pts = sub_elements[i];
            unsigned int cant = 0;
            for (unsigned int j=0; j<e_pts.size(); j++) {
                if (points->at(e_pts[j]).isInside() && !points->at(e_pts[j]).wasProjected()) {  //Notes: Verifica si el punto esta adentro
                    onein = true;
                    cant++;
                }
                
            }
            if (onein) {
                still_in.push_back(sub_elements[i]);
            }
            else {
                // to_review.push_back(sub_elements[i]);
                Point3D avg;
                for (unsigned int i =0; i<e_pts.size(); i++){
                    avg += points->at(o->getPoints()[i]).getPoint(); // El peso promedio del punto está dentro
                }
                avg /= e_pts.size();
                if (cant == 0) {  //Notes: Verifica si el punto esta adentro
                    for (unsigned int j=0; j<e_pts.size(); j++) {
                        if (points->at(e_pts[j]).wasProjected()){
                            cout << max_dis <<'\n';
                            cout << avg[0] <<','<< avg[1] <<','<< avg[2]<< " centroide \n";
                            cout << " ggg \n";
                            Point3D newP = newPointTowardsCentroide(points->at(e_pts[j]).getPoint(), max_dis, avg);
                            cout << newP[0] <<','<< newP[1] <<','<< newP[2]<< " new\n";
                            if (mesh->pointIsInMesh(newP, faces_inter)){
                                points->at(e_pts[j]).setInside();
                                return false;
                            }
                        }
                    }
                }
                // if (max_dis - (points->at(o->getPoints()[0]).getPoint() - avg).Norm() < -0.1) {
                //     cout << (points->at(o->getPoints()[0]).getPoint() - avg).Norm()<< " "<< max_dis << " "<< max_dis-(points->at(o->getPoints()[0]).getPoint() - avg).Norm()<<"\n";
                //     o->setSurface();
                //     for (unsigned int j=0; j<e_pts.size(); j++) { 
                //         points->at(e_pts[j]).setProjected();
                //     }
                //     return false;
                // }

                if (mesh->pointIsInMesh(avg, faces_inter)){
                    o->setSurface();
                    // for (unsigned int j=0; j<e_pts.size(); j++) { 
                    //     points->at(e_pts[j]).setProjected();
                    // }
                    // return false;
                } 
                else {
                     to_review.push_back(sub_elements[i]);
                }
            }
        }

        if (still_in.size()==sub_elements.size()) { //Notes: False if there's still elements inside.
            return false;
        }
        if (still_in.empty()) {
            // vector<Point3D> input_pts = mesh->getPoints();//1756
            // vector<SurfTriangle> faces = mesh->getFaces();
            // for (it=to_review.begin(); it!=to_review.end(); it++) {
            //     Point3D avg;
            //     for (unsigned int i =0; i<(*it).size(); i++){
            //         avg += points->at(o->getPoints()[i]).getPoint(); // El peso promedio del punto está dentro
            //     }
            //     avg /= (*it).size();

            //     // vector<vector<Point3D> > oct_edges = iv.getEdges(newPoint(points->at((*it)[0]).getPoint(), max_dis),
            //     //                                                 newPoint(points->at((*it)[6]).getPoint(), max_dis_neg));

            //     vector<vector<Point3D> > oct_edges = iv.getEdges(
            //         newPointTowardsCentroide(points->at((*it)[0]).getPoint(), max_dis, avg), 
            //         newPointTowardsCentroide(points->at((*it)[6]).getPoint(), max_dis, avg));

            //     vector<vector<Point3D> > edges;
            //     vector<Point3D> edge(2, Point3D ());
            //     edge[0] = points->at((*it)[0]).getPoint();
            //     edge[1] = points->at((*it)[6]).getPoint();
            //     edges.push_back(edge);
            //     // unsigned int count=0;
            //     for (auto j : faces_inter){
            //     // for (unsigned int j=0; j<faces_inter.size(); j++) {
            //         if (edgeTriangleIntersection(faces[j],input_pts,oct_edges)){
            //             o->setSurface();
            //             vector<unsigned int> e_pts = (*it);
            //             for (unsigned int k=0; k<e_pts.size(); k++) { 
            //                 points->at(e_pts[k]).setProjected();
            //             }
            //             return false; //Do not remove
            //         }
            //     }

            // }
            return true;

        }

        sub_elements.clear();
        sub_elements.reserve(still_in.size());
        for (iter=still_in.begin(); iter!=still_in.end(); iter++) {
            sub_elements.push_back(*iter);
        }
        return false;
    }
    bool RemoveSubElementsVisitor::edgeTriangleIntersection(SurfTriangle &st,
                                                   vector<Point3D> &input_pts,
                                                   vector<vector<Point3D>> &oct_edges) {

        //test each edge against the triangle
        for (unsigned int i=0; i<oct_edges.size(); i++) {
            vector<Point3D> oct_ed = oct_edges[i];
            if (st.segmentIntersection(input_pts,oct_ed[0],oct_ed[1])) {
                return true;
            }
        }

        return false;
    }

    Point3D RemoveSubElementsVisitor::newPoint(Point3D &point, double &dis) {
        Point3D newPoint;
        //Move the point in a distance dis.
        for (unsigned int i=0; i<3; i++) {
            newPoint[i] = point[i] + dis;
        }
        return newPoint;
    }

    Point3D RemoveSubElementsVisitor::newPointTowardsCentroide(Point3D &point, double &targetDistance, Point3D &centroid) {
        // Check for invalid distance
        // cout << point[0]<<"," << point[1]<<","<<point[2]<< " "<< centroid[0]<<"," << centroid[1]<<","<<centroid[2]<< " dis" << targetDistance <<" \n";
        if (targetDistance <= 0) {
            throw std::invalid_argument("Target distance must be positive.");
        }

        // Calculate the direction vector from 'point' to 'centroid'
        Point3D direction = centroid - point;

        // Normalize the direction vector
        // Move the point in the direction by the desired distance
        Point3D newPoint = point + direction.normalize() * targetDistance;
        // cout << newPoint<< "\n";
        return newPoint;
    }
}