#!/usr/bin/env bash
docker build -t hxp:linear -f Dockerfile.build .
CONT=$(docker run -d hxp:linear)
docker cp ${CONT}:/home/user/chall .
docker rm ${CONT}
