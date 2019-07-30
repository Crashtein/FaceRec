#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

double compareFaces(Mat rootFace);

int main()
{
    CascadeClassifier faceClas; //klasyfikator twarzy
    Mat rootImage,rootFace,compImage,compFace;
    vector<Rect> rootFaces,faces; //wektor na twarze
    double propWz=0,prop=0;    //wzorcowe proporcje,proporcje do porównania

    faceClas.load("E:\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_default.xml");
    rootImage=imread("E:\\dicaprio\\1.jpg");  //wczytanie zdjecia z twarzą bazową
    faceClas.detectMultiScale(rootImage,rootFaces); //wykrycie twarzy i zapis do wektora

    rootFace=Mat(rootImage,rootFaces[0]);   //wybor twarzy wzorca

    for(int i=0;i<rootFaces.size();i++){
        rectangle(rootImage,rootFaces[i],CV_RGB(0,255,0),2);
    }
    //wyswietl zdjecie z oznaczeniami
    imshow("RootImage",rootImage);

    if(rootFaces.size()==1){
        cout<<"Wzorzec dobry";
    }else{
        cout<<"Zly wzorzec";
    }

    //badanie twarzy wzorcowej
    propWz=compareFaces(rootFace);
    waitKey();

    compImage=imread("E:\\dicaprio\\5.png");    //obraz z twarzami do rozpoznania
    faceClas.detectMultiScale(compImage,faces);
    for(int i=0;i<faces.size();i++){
        prop=compareFaces(Mat(compImage,faces[i]));
        if(prop<1.1*propWz && prop>0.9*propWz){
            rectangle(compImage,faces[i],CV_RGB(0,255,0),2);
        }else{
            rectangle(compImage,faces[i],CV_RGB(255,0,0),2);
        }
        waitKey();
    }

    imshow("RecognizedFaces",compImage);
    waitKey();
    return 0;
}


double compareFaces(Mat rootFace){
    //Wykrywanie "dziurek" nosa
    Mat binFace;
    int j=0;
    int k=0;
    int binaryVal=20;
    vector <vector <Point> > contours;
    resize(rootFace,rootFace,Size(320,320), 0, 0);
    while((j==0 || k==0) && binaryVal<250){     //wykrywanie dopoki nie znajdzie elementow w opisanym zakresie lub nie znajdzie, a argument binaryzacji przekroczy 250
        j=0;
        k=0;
        cvtColor(rootFace,binFace,CV_BGR2GRAY);     //czarno-bialy
        threshold(binFace,binFace,binaryVal,255,THRESH_BINARY_INV);     //binaryzacja binaryVal zmienia się dla każdej iteracji while
        equalizeHist(binFace,binFace);      //rozciągnięcie histogramu
        morphologyEx(binFace,binFace,MORPH_ERODE,getStructuringElement(MORPH_ELLIPSE,Size(2,2)));   //erozja
        morphologyEx(binFace,binFace,MORPH_DILATE,getStructuringElement(MORPH_ELLIPSE,Size(2,2)));  //dylatacja
        Moments contMoment;
        findContours(binFace,contours,CV_RETR_LIST,1);  //zapis konturów do contours
        if(contours.size()!=0){
            for(int i =0; i<contours.size();i++){       //wyszukanie początkowego obiektu uważanego za lewa dziurke nosa
                contMoment=moments(contours[i]);
                if(contMoment.m01/contMoment.m00>180 && contMoment.m01/contMoment.m00<215 && contMoment.m10/contMoment.m00<=155
                        && contMoment.m10/contMoment.m00>=120 && contourArea(contours[i])>15 && j==0){  //warunki pozycji środka ciężkości oraz wielkości
                    j=i;
                    cout<<"j0: "<<j;
                }
            }
            for(int i =0; i<contours.size();i++){   //sprawdzanie czy nie ma lepszych kandydatów na ten obiekt, korzystam z wsp. Malinowskiej
                if((arcLength(contours[i],1)/(2*sqrt(3.14*contourArea(contours[i])))-1)<(arcLength(contours[j],1)/(2*sqrt(3.14*contourArea(contours[j])))-1)){
                    contMoment=moments(contours[i]);
                    if(contMoment.m01/contMoment.m00>180 && contMoment.m01/contMoment.m00<215 && contMoment.m10/contMoment.m00<=155 && contMoment.m10/contMoment.m00>=120 && contourArea(contours[i])>15){
                        j=i;
                        cout<<"j: "<<j;
                    }
                }
            }
            for(int i =0; i<contours.size();i++){   //to samo, symetrycznie dla dziurki po prawej
                contMoment=moments(contours[i]);
                if(contMoment.m01/contMoment.m00>180 && contMoment.m01/contMoment.m00<215 && contMoment.m10/contMoment.m00>=165 && contMoment.m10/contMoment.m00<=240 && contourArea(contours[i])>15 && i!=j && k==0){
                    k=i;
                    cout<<"k0: "<<k;
                }
            }
            for(int i =0; i<contours.size();i++){
                if((arcLength(contours[i],1)/(2*sqrt(3.14*contourArea(contours[i])))-1)<(arcLength(contours[k],1)/(2*sqrt(3.14*contourArea(contours[k])))-1) && i!=j){
                    contMoment=moments(contours[i]);
                    if(contMoment.m01/contMoment.m00>180 && contMoment.m01/contMoment.m00<215 && contMoment.m10/contMoment.m00>=165 && contMoment.m10/contMoment.m00<=240 && contourArea(contours[i])>15){
                        k=i;
                        cout<<"k: "<<k;
                    }
                }
            }
        }
        binaryVal+=5;   //zwiekszenie wartosci odcięcia dla f binaryzującej
    }
    //Analogicznie dla wykrywania oczu
    //zmiana warunkow obszarowych
    //priorytetem jest wysoka wartosc stosunku L/S, szukam charakterystycznych cieni powiek/rzęs/brwi
    int j2=0;
    int k2=0;
    int binaryVal2=20;
    vector <vector <Point> > contours2;
    while((j2==0 || k2==0) && binaryVal2<250){
        j2=0;
        k2=0;
        cvtColor(rootFace,binFace,CV_BGR2GRAY);
        threshold(binFace,binFace,binaryVal2,255,THRESH_BINARY_INV);
        equalizeHist(binFace,binFace);
        morphologyEx(binFace,binFace,MORPH_ERODE,getStructuringElement(MORPH_ELLIPSE,Size(2,2)));
        morphologyEx(binFace,binFace,MORPH_DILATE,getStructuringElement(MORPH_ELLIPSE,Size(2,2)));
        Moments contMoment;
        findContours(binFace,contours2,CV_RETR_LIST,1);
        if(contours2.size()!=0){
            for(int i =0; i<contours2.size();i++){
                contMoment=moments(contours2[i]);
                if(contMoment.m01/contMoment.m00>95 && contMoment.m01/contMoment.m00<150 && contMoment.m10/contMoment.m00<=130 && contMoment.m10/contMoment.m00>=70 && contourArea(contours2[i])>100 && j2==0){
                    j2=i;
                    cout<<"j20: "<<j2;
                }
            }
            for(int i =0; i<contours2.size();i++){
                if(arcLength(contours2[i],1)/contourArea(contours2[i])>arcLength(contours2[j2],1)/contourArea(contours2[j2])){
                    contMoment=moments(contours2[i]);
                    if(contMoment.m01/contMoment.m00>95 && contMoment.m01/contMoment.m00<150 && contMoment.m10/contMoment.m00<=130 && contMoment.m10/contMoment.m00>=70 && contourArea(contours2[i])>100){
                        j2=i;
                        cout<<"j2: "<<j2;
                    }
                }
            }
            for(int i =0; i<contours2.size();i++){
                contMoment=moments(contours2[i]);
                if(contMoment.m01/contMoment.m00>95 && contMoment.m01/contMoment.m00<150 && contMoment.m10/contMoment.m00>=190 && contMoment.m10/contMoment.m00<=250 && contourArea(contours2[i])>100 && i!=j2 && k2==0){
                    k2=i;
                    cout<<"k20: "<<k2;
                }
            }
            for(int i =0; i<contours2.size();i++){
                if(arcLength(contours2[i],1)/contourArea(contours2[i])>arcLength(contours2[k2],1)/contourArea(contours2[k2]) && i!=j2){
                    contMoment=moments(contours2[i]);
                    if(contMoment.m01/contMoment.m00>95 && contMoment.m01/contMoment.m00<150 && contMoment.m10/contMoment.m00>=190 && contMoment.m10/contMoment.m00<=250 && contourArea(contours2[i])>100){
                        k2=i;
                        cout<<"k2: "<<k2;
                    }
                }
            }
        }
        binaryVal2+=5;
    }


    //rysunek wykrytych czesci
    drawContours(rootFace,contours,j,CV_RGB(0,0,255),5);
    drawContours(rootFace,contours,k,CV_RGB(0,255,0),5);
    drawContours(rootFace,contours2,j2,CV_RGB(255,0,0),5);
    drawContours(rootFace,contours2,k2,CV_RGB(255,0,255),5);
    //j-obiekt z lewej, k-obiekt z prawej; brak indeksu-dziurki nosa,indeks2-oczy
    Moments momL,momP,mom2L,mom2P;
    double prop;
    momL=moments(contours[j]);
    momP=moments(contours[k]);
    mom2L=moments(contours2[j2]);
    mom2P=moments(contours2[k2]);
    //odleglosc miedzy oczami
    // Moment.m01/Moment.m00 //y
    // Moment.m10/Moment.m00 //x
    prop=(sqrt((mom2P.m10/mom2P.m00-mom2L.m10/mom2L.m00)*(mom2P.m10/mom2P.m00-mom2L.m10/mom2L.m00)+(mom2P.m10/mom2P.m00-mom2L.m10/mom2L.m00)*(mom2P.m10/mom2P.m00-mom2L.m10/mom2L.m00)))    //odleglosc oczu od siebie przez:
            /sqrt((((momP.m10/momP.m00+momL.m10/momL.m00)/2)-((mom2P.m10/mom2P.m00+mom2L.m10/mom2L.m00)/2))*(((momP.m10/momP.m00+momL.m10/momL.m00)/2)-((mom2P.m10/mom2P.m00+mom2L.m10/mom2L.m00)/2))   //odleglosc lini oczu
            +(((momP.m01/momP.m00+momL.m01/momL.m00)/2)-((mom2P.m01/mom2P.m00+mom2L.m01/mom2L.m00)/2))*(((momP.m01/momP.m00+momL.m01/momL.m00)/2)-((mom2P.m01/mom2P.m00+mom2L.m01/mom2L.m00)/2)));      //od konca nosa
    cout<<'\n'<<"Proporcje: "<<prop<<'\n';

    imshow("BinaryRootFaceForEyeDetection",binFace);
    imshow("RootFace",rootFace);
    return prop;
}
