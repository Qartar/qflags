# Configures Readme.md to ensure it stays up to date with the code.
file(READ ${PROJECT_SOURCE_DIR}/sample/trivial.cpp TRIVIAL_CPP)
string(STRIP "${TRIVIAL_CPP}" TRIVIAL_CPP)
configure_file(${PROJECT_SOURCE_DIR}/cmake/modules/Readme.conf ${PROJECT_SOURCE_DIR}/Readme.md)
