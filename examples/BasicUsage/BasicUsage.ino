#include <TreadmillRemote.h>

TreadmillRemote treadmillRemote(D2);

void setup() {
    treadmillRemote.begin();
    
    treadmillRemote.start();
}

void loop() {
    // Nothing here
}