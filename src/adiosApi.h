#include <string>
#include <vector>

template <class T>
class adiosApi_impl;

template <class T>
class adiosApi {

  public:
    adiosApi(std::string fn="adios-writer.bp");
    ~adiosApi();

    void addPoints(const T*, const T*, const T*, size_t);
    void addPointData(const std::string &, const T*);
    void addCell(size_t *, size_t, int offset = 0);

  private:
    adiosApi_impl<T>* impl;
};
