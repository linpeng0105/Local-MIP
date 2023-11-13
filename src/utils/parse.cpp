#include <cstring>
#include <fstream>
#include "header.h"
#include "paras.h"

Integer paras::identify_opt(const char *file)
{
    char name[strlen(file) + 1], p = -1, l = strlen(file);
    for (int i = l - 1; i >= 0; i--)
        if (file[i] == '/')
        {
            p = i;
            break;
        }
    strncpy(name, file + p + 1, l - p - 1);
    name[l - p - 1] = '\0';
    printf("c File name (with path): %s\n", file);
    printf("c File name: %s\n", name);
    if (strcmp(name, "2club200v15p5scn.mps") == 0)
        return (Integer)((-70) * (ZoomTimes));
    if (strcmp(name, "academictimetablebig.mps") == 0)
        return (Integer)((427) * (ZoomTimes));
    if (strcmp(name, "bab1.mps") == 0)
        return (Integer)((-218764.8853) * (ZoomTimes));
    if (strcmp(name, "bab2.mps") == 0)
        return (Integer)((-357544.3115) * (ZoomTimes));
    if (strcmp(name, "bab6.mps") == 0)
        return (Integer)((-284248.2307) * (ZoomTimes));
    if (strcmp(name, "comp21-2idx.mps") == 0)
        return (Integer)((74) * (ZoomTimes));
    if (strcmp(name, "ds.mps") == 0)
        return (Integer)((93.52) * (ZoomTimes));
    if (strcmp(name, "fhnw-sq2.mps") == 0)
        return (Integer)((0) * (ZoomTimes));
    if (strcmp(name, "graph20-80-1rand.mps") == 0)
        return (Integer)((-6) * (ZoomTimes));
    if (strcmp(name, "hanoi5.mps") == 0)
        return (Integer)((1931) * (ZoomTimes));
    if (strcmp(name, "highschool1-aigio.mps") == 0)
        return (Integer)((0) * (ZoomTimes));
    if (strcmp(name, "ivu52.mps") == 0)
        return (Integer)((481.0068) * (ZoomTimes));
    if (strcmp(name, "neos-3214367-sovi.mps") == 0)
        return (Integer)((179965) * (ZoomTimes));
    if (strcmp(name, "neos-3237086-abava.mps") == 0)
        return (Integer)((0) * (ZoomTimes));
    if (strcmp(name, "neos-3530905-gaula.mps") == 0)
        return (Integer)((159) * (ZoomTimes));
    if (strcmp(name, "neos-4165869-wannon.mps") == 0)
        return (Integer)((293) * (ZoomTimes));
    if (strcmp(name, "neos-4954274-beardy.mps") == 0)
        return (Integer)((20946.48) * (ZoomTimes));
    if (strcmp(name, "neos-954925.mps") == 0)
        return (Integer)((-237.7688889) * (ZoomTimes));
    if (strcmp(name, "nursesched-medium-hint03.mps") == 0)
        return (Integer)((115) * (ZoomTimes));
    if (strcmp(name, "opm2-z10-s4.mps") == 0)
        return (Integer)((-33269) * (ZoomTimes));
    if (strcmp(name, "opm2-z12-s8.mps") == 0)
        return (Integer)((-58540) * (ZoomTimes));
    if (strcmp(name, "proteindesign121hz512p9.mps") == 0)
        return (Integer)((1473) * (ZoomTimes));
    if (strcmp(name, "pythago7824.mps") == 0)
        return (Integer)((0) * (ZoomTimes));
    if (strcmp(name, "queens-30.mps") == 0)
        return (Integer)((-40) * (ZoomTimes));
    if (strcmp(name, "rail03.mps") == 0)
        return (Integer)((-867.0939654) * (ZoomTimes));
    if (strcmp(name, "reblock354.mps") == 0)
        return (Integer)((-39280521.23) * (ZoomTimes));
    if (strcmp(name, "rococoC11-010100.mps") == 0)
        return (Integer)((20889) * (ZoomTimes));
    if (strcmp(name, "rococoC12-010001.mps") == 0)
        return (Integer)((34045) * (ZoomTimes));
    if (strcmp(name, "s100.mps") == 0)
        return (Integer)((-0.169723527) * (ZoomTimes));
    if (strcmp(name, "s1234.mps") == 0)
        return (Integer)((29) * (ZoomTimes));
    if (strcmp(name, "sorrell3.mps") == 0)
        return (Integer)((-16) * (ZoomTimes));
    if (strcmp(name, "splice1k1.mps") == 0)
        return (Integer)((-394) * (ZoomTimes));
    if (strcmp(name, "splice1k1i.mps") == 0)
        return (Integer)((-394) * (ZoomTimes));
    if (strcmp(name, "stp3d.mps") == 0)
        return (Integer)((493.71965) * (ZoomTimes));
    if (strcmp(name, "supportcase10.mps") == 0)
        return (Integer)((7) * (ZoomTimes));
    if (strcmp(name, "supportcase19.mps") == 0)
        return (Integer)((12677206) * (ZoomTimes));
    if (strcmp(name, "supportcase3.mps") == 0)
        return (Integer)((0) * (ZoomTimes));
    if (strcmp(name, "tw-myciel4.mps") == 0)
        return (Integer)((10) * (ZoomTimes));
    if (strcmp(name, "v150d30-2hopcds.mps") == 0)
        return (Integer)((41) * (ZoomTimes));
    if (strcmp(name, "wnq-n100-mw99-14.mps") == 0)
        return (Integer)((259) * (ZoomTimes));
    if (strcmp(name, "woodlands0.mps") == 0)
        return (Integer)((0) * (ZoomTimes));
    return MinValue;
}
