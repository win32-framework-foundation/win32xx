/////////////////////////////
// Doc.h
//

#ifndef WIN32XX_DOC_H
#define WIN32XX_DOC_H

///////////////////////////////////////
// Declaration of the PlotPoint struct.
//
struct PlotPoint
{
    int x;
    int y;
    bool isPenDown;
    COLORREF penColor;
};

//////////////////////////////////////////////////////////////
// Declaration of the CDoc class.
// CDoc holds the application's data. It inherits from CObject
// to perform data serialization to and from the archive.
//
class CDoc : public CObject
{
public:
    CDoc() {}
    ~CDoc() {}

    std::vector<PlotPoint>& GetAllPoints() {return m_points;}   // returns a vector of PlotPoint data
    void FileOpen(LPCTSTR filename);
    void FileSave(LPCTSTR filename);
    void Serialize(CArchive &ar);
    void StorePoint(int x, int y, bool isPenDown, COLORREF penColor);

private:
    std::vector<PlotPoint> m_points;    // Points of lines to draw
};

#endif  // WIN32XX_DOC_H