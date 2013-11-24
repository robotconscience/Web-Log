//
//  Fireplace.h
//  LogFlickr
//
//  Created by BRenfer on 11/6/13.
//
//

#pragma once

#include "ofMain.h"
#include "Searcher.h"

class Fireplace : public Searcher {
public:
    
    Fireplace() : Searcher() {
        term = "hearth";
        x = 960;
        y = 540;
        width = 1920;
        height = 1080;
        tint.set(150);
        changeRate = 1000;
        
    }
};
