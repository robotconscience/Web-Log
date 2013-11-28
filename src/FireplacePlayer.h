//
//  Searcher.h
//  LogFlickr
//
//  Created by BRenfer on 11/6/13.
//
//

#pragma once

#include "ofMain.h"
#include "Searcher.h"

class FireplacePlayer : public Searcher {
public:
    
    FireplacePlayer() : Searcher() {
        term = "fire";
        x = 960;
        y = 540;
        width = 1920;
        height = 1080;
        changeRate = 1000;
        media = "videos";
        changeRateMin = 1000;
        changeRateMax = 1000;
    }
    
protected:
    
    vector<ofVideoPlayer*> players;
};