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

    bool RemoveSubElementsVisitor::visit(Octant *o) { //Notes:Visita octante
        vector<vector<unsigned int>> &sub_elements = o->sub_elements;

        list<vector<unsigned int> > still_in;
        list<vector<unsigned int> >::iterator iter;
        // cout << "points_size: " << points[1].size() << "\n";
        // cout << "sub elements size: " << sub_elements.size() << "\n";
        for (unsigned int i=0; i<sub_elements.size(); i++) {
            bool onein = false;
            vector<unsigned int> e_pts = sub_elements[i];
            //notes: 
            // cout << "e pts size: " << e_pts.size() << " proj: ";
            int proj = 0;
            for (unsigned int j=0; j<e_pts.size(); j++) {
                // cout << "e_pts " << j << ": "<< e_pts[j] << " ";
                if (points->at(e_pts[j]).isInside()) {  //Notes: Verifica si el punto esta adentro
                    // cout << "points[0]["<< e_pts[j]<<"]: ("<< points[0][e_pts[j]]<< ") ";
                    // cout << "T, ";
                    onein = true;
                    break;
                }
                if (points->at(e_pts[j]).wasProjected()) { 
                    proj+=1;
                }
                else{
                    // cout << "F, ";
                }
            }
            // cout << proj << "\n";
            if (onein) {
                still_in.push_back(sub_elements[i]);
            }
        }

        //Notes:
        //for i in sub_elements:
        //  for j in e_pts:
        //    if j inside:
        //      onein = true
        //  if onein: 
        //    sill_in.append(i)

        if (still_in.size()==sub_elements.size()) { //Notes: False si quedan elementos
            return false;
        }
        if (still_in.empty()) {
            return true;
        }

        sub_elements.clear();
        sub_elements.reserve(still_in.size());
        for (iter=still_in.begin(); iter!=still_in.end(); iter++) {
            sub_elements.push_back(*iter);
        }
        return false;
    }
}