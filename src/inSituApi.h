#include <string>
#include <vector>

template <class T>
class inSituApi_impl;

template <class T>
class inSituApi {

  public:
    inSituApi(std::string fn="inSitu-writer");
    ~inSituApi();

    void addPoints(const T*, const T*, const T*, int, int, int);
    void addPointData(const std::string &, const T*);
    void addCells(size_t *, int, int, int, int);

  private:
    inSituApi_impl<T>* impl;
};
