#include "slidesystem.h"
#include <QDebug>

SlideSystem::SlideSystem(int countTeams)
{
    this->countTeams = countTeams;
    this->matches.clear();

    QList<int> l1;
    QList<int> l2;

    l1.clear();
    l2.clear();

    if(this->countTeams % 2 != 0){
        // |x|1|2| --> Pos x is static
        // |5|4|3|
        //
        // |x|5|1| --> Pos x is static
        // |4|3|2|

        // init. list 1 & 2
        l1.append(0); // insert Position x

        for(int i = 1; i <= countTeams/2; i++){
            l1.append(i);
        }

        for(int i = countTeams; i > countTeams/2; i--){
            l2.append(i);
        }

        // Create Matches
        bool firstRun = true;
        while(l1.at(1) != 1 || firstRun){
            firstRun = false;

            for(int i = 1; i < l1.size(); i++){
                matches.append(QPair<int,int>(l1.at(i),l2.at(i)));
            }

            int l1LastBackup = l1.last();
            int l2LastBackup = l2.first();

            l1.pop_back();
            l1.insert(1, l2LastBackup);
            l2.pop_front();
            l2.append(l1LastBackup);
        }

    }else{
        // |1|2|3| --> Pos 1 is static
        // |6|5|4|
        //
        // |1|6|2| --> Pos 1 is static
        // |5|4|3|

        for(int i = 1; i <= countTeams/2; i++){
            l1.append(i);
        }

        for(int i = countTeams; i > countTeams/2; i--){
            l2.append(i);
        }

        // Create Matches
        bool firstRun = true;
        while(l1.at(1) != 2 || firstRun){
            firstRun = false;

            for(int i = 0; i < l1.size(); i++){
                matches.append(QPair<int,int>(l1.at(i),l2.at(i)));
            }

            int l1LastBackup = l1.last();
            int l2LastBackup = l2.first();

            l1.pop_back();
            l1.insert(1, l2LastBackup);
            l2.pop_front();
            l2.append(l1LastBackup);
        }
    }
    matchIterator = matches.begin();
}

QPair<int,int> SlideSystem::getNextMatch()
{
    // Return if Iterator is at end
    if(matchIterator == matches.end()){
        return QPair<int,int>(0,0);
    }

    // Return Match
    QPair<int,int> current(matchIterator->first, matchIterator->second);
    matchIterator++;
    return current;
}

void SlideSystem::resetIterator()
{
    matchIterator = matches.begin();
}
