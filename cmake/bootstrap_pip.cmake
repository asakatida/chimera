find_program(CURL "curl")
if(CURL)
 execute_process(
   COMMAND ${CURL} https://bootstrap.pypa.io/get-pip.py
   COMMAND chimera -
   ENCODING UTF8)
endif()
