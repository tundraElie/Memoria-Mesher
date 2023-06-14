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

        list<vector<unsigned int> > still_in;
        list<vector<unsigned int> > to_review;
        list<vector<unsigned int> >::iterator iter;
        list<vector<unsigned int> >::iterator it;
        IntersectionsVisitor iv; // visitors

        // if (o->wasProjected()) { 
        //     cout <<  "hi \n";
        // }
        for (unsigned int i=0; i<sub_elements.size(); i++) {
            bool onein = false;
            vector<unsigned int> e_pts = sub_elements[i];
            for (unsigned int j=0; j<e_pts.size(); j++) {
                // if (points->at(e_pts[j]).wasProjected()) { 
                //     list<unsigned int> p_eles = points->at(e_pts[j]).getElements();
                //     list<unsigned int>::iterator peiter;
                //     for (peiter=p_eles.begin(); peiter!=p_eles.end(); peiter++) {
                //         if (o[*peiter].getProjectedNodes() > 7){
                //             cout << o[*peiter].getProjectedNodes() << " ";
                //             o[*peiter].setProjected();
                //         }
                //     }
                //     // cout <<  " \n";
                // }
                if (points->at(e_pts[j]).isInside()) {  //Notes: Verifica si el punto esta adentro
                    onein = true;
                    break;
                }
            }
            // cout <<  " Q\n";
            if (onein) {
                still_in.push_back(sub_elements[i]);
            }
            else {
                if (o->isSurfaceAndInside()) { 
                    cout <<  "hi ";
                    return false;
                }
                to_review.push_back(sub_elements[i]);
            }
        }

        if (still_in.size()==sub_elements.size()) { //Notes: False if there's still elements inside.
            return false;
        }
        if (still_in.empty()) {
            vector<SurfTriangle> faces = mesh->getFaces();//3524
            vector<Point3D> input_pts = mesh->getPoints();//1756
            for (it=to_review.begin(); it!=to_review.end(); it++) {
                vector<vector<Point3D> > oct_edges = iv.getEdges(points->at((*it)[0]).getPoint(),
                                                                points->at((*it)[6]).getPoint());
                // for (unsigned int j=0; j<faces.size(); j++) {
                //     if (edgeTriangleIntersection(faces[j],input_pts,oct_edges)){
                //         // return false; //Do not remove
                //         // cout <<  " \n";
                //     }
                // }
            }
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
}