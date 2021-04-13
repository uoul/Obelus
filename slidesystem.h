#ifndef SLIDESYSTEM_H
#define SLIDESYSTEM_H

#include <QPair>
#include <QVector>


class SlideSystem
{
public:
    SlideSystem(int countTeams);

    QPair<int,int> getNextMatch();

    void resetIterator();

private:
    int currentIndex;
    int countTeams;
    QVector<QPair<int,int>> matches;
    QVector<QPair<int,int>>::iterator matchIterator;

};

#endif // SLIDESYSTEM_H
