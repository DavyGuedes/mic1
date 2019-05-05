# GCC support can be specified at major, minor, or micro version
# (e.g. 8, 8.2 or 8.2.0).
# See https://hub.docker.com/r/library/gcc/ for all supported GCC
# tags from Docker Hub.
# See https://docs.docker.com/samples/library/gcc/ for more on how to use this image
FROM gcc:latest as build    

# These commands copy your files into the specified directory in the image
# and set that as the working location
COPY . /usr/src/mic1
WORKDIR /usr/src/mic1

# This command compiles your app using GCC, adjust for your source code
RUN gcc -o Emulador.o Emulador.c

# This command starts a new image from python:3 to run the assembler.py
FROM python:3

# These commands copy files from initial image (as  build) into the specified directory in the new image 
# and set that as the working location
COPY --from=build /usr/src/mic1 /usr/src/mic1
WORKDIR /usr/src/mic1

LABEL Name=mic1 Version=0.0.1    

# This command runs your application, comment out this line to compile only
# CMD ["./Emulador.o"]
CMD ["bash"]
