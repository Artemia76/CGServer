# CGServer

ChatGlobal server for Activeworlds Universe

1. DEPENDENCIES

    - wxWidgets : >=3.0.0
    - ActiveWorlds SDK for linux

1. Build on Linux
    1. UBUNTU, MINT

        On ubuntu you can enter this command line to install needed dependencies:
            ```
            sudo apt-get install build-essential libwxbase3.0-dev
            ```
        Install AW SDK lib in your local lib32 folder, for exemple on ubuntu/mint :
            ```
            ls /usr/local/lib/i386-linux-gnu
            >libaw_sdk.51.so.93
            sudo ldconfig
            ls /usr/local/lib/i386-linux-gnu
            >libaw_sdk.51.so  libaw_sdk.51.so.93
            ```
        Install AW SDK Include file Aw.h :
            ```
            ls /usr/local/include
            >Aw.h
            ```

    1. Final build:

        on git root :
            ```
            mkdir build
            cd build
            cmake .. -DAW_SDK=93 -DBUILD_32=ON
            make
            sudo make install
            ```

    1. Update

        repeat previous commands after git pull

    1. Configure
        coming ..

1. Windows

    coming ...
