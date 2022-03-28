#ifndef example_oatpp_mongo_Model_hpp
#define example_oatpp_mongo_Model_hpp

#include "dto/DTOs.hpp"

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

namespace db {

class AudioSample : public oatpp::DTO {

  DTO_INIT(AudioSample, DTO)

  DTO_FIELD(String, _id);
  DTO_FIELD(Int16, numTransients);
  DTO_FIELD(String, url);
  DTO_FIELD(String, artist);
  DTO_FIELD(String, albumPack);
  DTO_FIELD(String, sampleName);

};

}

#include OATPP_CODEGEN_END(DTO)

#endif // example_oatpp_mongo_Model_hpp