#########################################################################################################################
# TODO: REPLACE THE CONTENT OF THIS FILE WITH YOUR OWN.
#########################################################################################################################

# Use the official Python base image
FROM python:3.12-bookworm


# Installing Flask
ENV DEBIAN_FRONTEND=noninteractive

# Install only what we need
RUN apt-get update && apt-get install -y \
    gcc \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /chall

COPY server.c .
COPY flag.txt .
COPY alice.py .
COPY bob.py   .
COPY run.sh   .

RUN gcc -o server server.c
RUN chmod +x run.sh


# Expose the port that the chall will run on
EXPOSE 6667

# Run the chall
CMD ["./run.sh"]
