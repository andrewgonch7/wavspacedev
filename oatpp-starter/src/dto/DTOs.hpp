#ifndef DTOs_hpp
#define DTOs_hpp

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 *  Data Transfer Object. Object containing fields only.
 *  Used in API for serialization/deserialization and validation
 */
class MyDto : public oatpp::DTO {
  
  DTO_INIT(MyDto, DTO)
  
  DTO_FIELD(Int32, statusCode);
  DTO_FIELD(String, message);
  
};

class AudioDto : public oatpp::DTO {
  
  DTO_INIT(AudioDto, DTO)
  
  DTO_FIELD(Int32, statusCode);
  DTO_FIELD(Int16, numTransients);
  DTO_FIELD(String, url);
  
};

class AudioMetaDto : public oatpp::DTO {
  
  DTO_INIT(AudioMetaDto, DTO)
  
  DTO_FIELD(String, _id);
  DTO_FIELD(Int16, numTransients);
  DTO_FIELD(String, url);
  DTO_FIELD(String, artist);
  DTO_FIELD(String, albumPack);
  DTO_FIELD(String, sampleName);
  
};

class HTTP_Response : public oatpp::DTO {
  DTO_INIT(HTTP_Response, DTO)

  DTO_FIELD(Int16, statusCode);
  DTO_FIELD(String, message);
};

#include OATPP_CODEGEN_END(DTO)

#endif /* DTOs_hpp */
