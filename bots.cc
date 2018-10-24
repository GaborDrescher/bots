#include "real.h"
#include "bot.h"
#include "random.h"
#include "physics.h"
#include "graphics.h"
#include "timing.h"

#include <iostream>
using namespace std;

int main()
{
    int nbots = 10000;
    int width = 1000;
    int height = 1000;

    Bot bots[nbots];
    Random rand;

    for(int i = 0; i < nbots; ++i)
    {
        Bot &b = bots[i];
        b.pos.x = rand.get() % (width/2);
        b.pos.y = rand.get() % height;
        b.vel.x = ((rand.get() % 100000)/real(50000)) - real(1);
        b.vel.y = ((rand.get() % 100000)/real(50000)) - real(1);
        b.vel.x *= 3;
        b.vel.y *= 3;
        b.radius = 3;
        //b.radius = (rand.get() % 20) + 1;
        b.invmass = real(1) / (b.radius * real(2) * real(3.141));
    }

    Physics physics;
    Graphics graphics(width, height);

    uint64_t sumtime = 0;
    for(unsigned int i = 0; ; ++i)
    {
        uint64_t time = Time::getMillis();
        physics.tick(bots, nbots, real(0.1), width, height);
        time = Time::getMillis() - time;
        cerr << time << endl;
        sumtime += time;
        
        if(time == 0) sumtime += 1;

        if(sumtime > 40)
        {
            sumtime = 0;
            graphics.paint(bots, nbots);
        }
    }

    return 0;
}

