#pragma once

#include "real.h"
#include "botmath.h"
#include "bot.h"

#include <stdlib.h>
#include <string.h>
#include <iostream>
using namespace std;

#define EPS real(0.0001)
#define restitution real(0.9)
class Physics
{
    private:
    void force(Bot &a, Bot &b)
    {
        if(&a == &b) return;

        vec d = b.pos - a.pos;
        //distance between circle centres, squared
        real distance_squared = d.lengthSq();
        //if they are at the 'same' place, dont do anything
        if(distance_squared < EPS && distance_squared > (-EPS)) return;
        //combined radius squared
        real radius = a.radius + b.radius;
        real radius_squared = radius * radius;
        //circles too far apart
        if(distance_squared > radius_squared) return;
        //distance between circle centres
        real distance = sqrt(distance_squared);
        //normal of collision
        real distinv = real(1) / distance;
        vec ncoll = d * distinv;
        //penetration distance
        real dcoll = (radius - distance);
        //separation vec
        vec separation_vec = ncoll * (dcoll / (a.invmass + b.invmass)) * real(0.2);
        //separate the circles
        a.pos = a.pos - (separation_vec * a.invmass);
        b.pos = b.pos + (separation_vec * b.invmass);
        //combined velocity
        vec vcoll = b.vel - a.vel;
        //impact speed
        real vn = vcoll * ncoll;
        // obejcts are moving away. dont reflect velocity
        if(vn >= real(0)) return;
        //collision impulse
        real j = -(real(1) + restitution) * (vn) / (a.invmass + b.invmass);
        //collision impusle vec
        vec impulse = ncoll * j;
        //change momentum of the circles
        a.vel = a.vel - (impulse * a.invmass);
        b.vel = b.vel + (impulse * b.invmass);
    }

    void getCell(real x, real y, uint64_t &xc, uint64_t &yc)
    {
        xc = (x/width)*((width/cellsize) + 1);
        yc = (y/height)*((height/cellsize) + 1);
    }

    uint64_t cantorPair(uint64_t x, uint64_t y)
    {
        return ((x+y)*(x+y+1))/2 + y;
    }

    uint64_t cellhash(uint64_t x, uint64_t y)
    {
        return cantorPair(x, y) % hashsize;
    }
    uint64_t hash(real x, real y)
    {
        uint64_t nx;
        uint64_t ny;
        getCell(x,y,nx,ny);
        return cellhash(nx, ny);
    }

    void collide(Bot bots[], int nbots)
    {
        cellsize = bots[0].radius * real(2);

        if(hashsize < nbots * 2)
        {
            hashsize = nbots * 2;
            free(hashtable);
            hashtable = (Bot**)calloc(hashsize, sizeof(Bot*));
        }
        else
        {
            memset(hashtable, 0, hashsize*sizeof(Bot*));
        }

        for(int i = 0; i < nbots; ++i)
        {
            Bot *bot = &bots[i];
            uint64_t index = hash(bot->pos.x, bot->pos.y);
            bot->next = hashtable[index];
            hashtable[index] = bot;
        }
        
        //for(uint64_t i = 0; i != hashsize; ++i)
        //{
        //  int sum = 0;
        //  for(Bot *curr = hashtable[i]; curr != 0; curr = curr->next)
        //  {
        //      sum += 1;
        //  }
        //  if(sum != 0)
        //      cerr << i << ": " << sum << endl;
        //}
        //exit(0);

        //#pragma omp parallel for
        for(int i = 0; i < nbots; ++i)
        {
            Bot &bot = bots[i];
            uint64_t my_x;
            uint64_t my_y;
            getCell(bot.pos.x, bot.pos.y, my_x, my_y);
        
            uint64_t begin_x = my_x == 0 ? 0 : (my_x - 1);
            uint64_t begin_y = my_y == 0 ? 0 : (my_y - 1);

            uint64_t end_x = (my_x == (width/cellsize))  ? (width/cellsize)  : (my_x + 1);
            uint64_t end_y = (my_y == (height/cellsize)) ? (height/cellsize) : (my_y + 1);

            for(uint64_t y = begin_y; y <= end_y; ++y)
            {
                for(uint64_t x = begin_x; x <= end_x; ++x)
                {
                    Bot *other = hashtable[cellhash(x,y)];
                    for(;other != 0; other = other->next)
                    {
                        force(bot, *other);
                    }
                }
            }
        }
        


        ////#pragma omp parallel for
        //for(int i = 0; i < nbots; ++i)
        //{
        //  Bot &bot = bots[i];
        //  for(int k = i+1; k < nbots; ++k)
        //  {
        //      Bot &otherbot = bots[k];
        //      force(bot, otherbot);
        //  }
        //}
    }

    void walls(Bot bots[], int nbots)
    {
        for(int i = 0; i < nbots; ++i)
        {
            Bot &bot = bots[i];
            real radius = bot.radius;
            int w = width - radius;
            int h = height - radius;
            if(bot.pos.x < radius)
            {
                bot.pos.x = radius;
                if(bot.vel.x < 0)
                    bot.vel.x = -(bot.vel.x * restitution);
            }
            if(bot.pos.y > h)
            {
                bot.pos.y = h;
                if(bot.vel.y > 0)
                    bot.vel.y = -(bot.vel.y * restitution);
            }
            if(bot.pos.x > w)
            {
                bot.pos.x = w;
                if(bot.vel.x > 0)
                    bot.vel.x = -(bot.vel.x * restitution);
            }
            if(bot.pos.y < radius)
            {
                bot.pos.y = radius;
                if(bot.vel.y < 0)
                    bot.vel.y = -(bot.vel.y * restitution);
            }
        }
    }

    void advance(Bot bots[], int nbots, real dt)
    {
        const vec zero;
        for(int i = 0; i < nbots; ++i)
        {
            bots[i].vel = bots[i].vel + bots[i].force * (bots[i].invmass * dt);
            bots[i].pos = bots[i].pos + bots[i].vel * dt;
            bots[i].force = zero;
        }
    }

    void gravity(Bot bots[], int nbots)
    {
        for(int i = 0; i < nbots; ++i)
        {
            Bot &bot = bots[i];
            bot.force = bot.force + vec(0, 10);
        }
    }

    Bot **hashtable;
    uint64_t hashsize;
    int width;
    int height;
    int cellsize;

    public:
    Physics()
    {
        cellsize = 20;
        hashsize = 100;
        hashtable = (Bot**)calloc(hashsize, sizeof(Bot*));
    }

    void tick(Bot bots[], int nbots, real dt, int w, int h)
    {
        width = w;
        height = h;

        gravity(bots, nbots);
        for(int i = 0; i < 10; ++i) {
            collide(bots, nbots);
        }
        walls(bots, nbots);
        advance(bots, nbots, dt);
    }
};

