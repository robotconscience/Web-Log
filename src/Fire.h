//
//  Fire.h
//  LogFlickr
//
//  Created by BRenfer on 11/6/13.
//
//

#pragma once

#include "ofMain.h"
#include "Searcher.h"

class Fire : public Searcher {
public:
    
    Fire() : Searcher() {
        term = "fire";
        x = 960;
        y = 418;
        width = 642;
        height = 450;
        tint.set(255,156,0);
    }
};
