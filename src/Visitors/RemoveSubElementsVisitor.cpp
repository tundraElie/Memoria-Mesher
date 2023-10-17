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

        double max_dis = o->getMaxDistance()*0.99;
        double max_dis_neg = -max_dis;
        for (unsigned int i=0; i<sub_elements.size(); i++) {
            bool onein = false;
            vector<unsigned int> e_pts = sub_elements[i];
            for (unsigned int j=0; j<e_pts.size(); j++) {
                if (points->at(e_pts[j]).isInside() && !points->at(e_pts[j]).wasProjected() ) {  //Notes: Verifica si el punto esta adentro
                    onein = true;
                    break;
                }
            }
            // cout <<  " Q\n";
            if (onein) {
                o->setSurface();
                still_in.push_back(sub_elements[i]);
            }
            else {
                // to_review.push_back(sub_elements[i]);

                Point3D avg;
                for (unsigned int i =0; i<e_pts.size(); i++){
                    avg += points->at(o->getPoints()[i]).getPoint(); // El peso promedio del punto está dentro
                }
                avg /= e_pts.size();
               
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
                    for (unsigned int j=0; j<e_pts.size(); j++) { 
                        points->at(e_pts[j]).setProjected();
                    }
                    return false;
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
            vector<Point3D> input_pts = mesh->getPoints();//1756
            vector<SurfTriangle> faces = mesh->getFaces();
            // cout <<  faces.size() << " \n";
            // for (it=to_review.begin(); it!=to_review.end(); it++) {
            //     vector<vector<Point3D> > oct_edges = iv.getEdges(newPoint(points->at((*it)[0]).getPoint(), max_dis),
            //                                                     newPoint(points->at((*it)[6]).getPoint(), max_dis_neg));

            //     vector<vector<Point3D> > edges;
            //     vector<Point3D> edge(2, Point3D ());
            //     edge[0] = points->at((*it)[0]).getPoint();
            //     edge[1] = points->at((*it)[6]).getPoint();
            //     edges.push_back(edge);
            //     // unsigned int count=0;
            //     for (auto j : faces_inter){
            //     // for (unsigned int j=0; j<faces_inter.size(); j++) {
            //         if (edgeTriangleIntersection(faces[j],input_pts,oct_edges)){
            //             // count++;
            //             // cout <<  count << " ";
            //             o->setSurface();
            //             vector<unsigned int> e_pts = (*it);
            //             for (unsigned int k=0; k<e_pts.size(); k++) { 
            //                 points->at(e_pts[k]).setProjected();
            //             }
            //             return false; //Do not remove
            //         }
            //     }
            //     // // cout <<  "\n";
            //     // if (count >0){
            //     //     // cout <<  count << " ";
            //     //     return false; //Do not remove
            //     // }
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
            // cout << newPoint[i] << " ";
        }
        // cout  << "\n";
        return newPoint;
    }
}