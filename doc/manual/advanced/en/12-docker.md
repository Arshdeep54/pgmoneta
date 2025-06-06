\newpage

# Running pgmoneta with Docker

You can run [**pgmoneta**][pgmoneta] using Docker instead of compiling it manually.

## Prerequisites

* [**Docker**][docker] or [**Podman**][podman] must be installed on the server where PostgreSQL is running.
* Ensure PostgreSQL is configured to allow external connections.

## Update the configuration file if needed:

```ini
[pgmoneta]
host = *
metrics = 5001

base_dir = /home/pgmoneta/backup

compression = zstd

retention = 7

log_type = file
log_level = info
log_path = /tmp/pgmoneta.log

unix_socket_dir = /tmp/

[primary]
host = localhost
port = 5432
user = repl
wal_slot = repl
```

## Step 1: Enable External PostgreSQL Access

Modify the local PostgreSQL server's `postgresql.conf` file to allow connections from outside:
```ini
listen_addresses = '*'
```

Update `pg_hba.conf` to allow remote connections:
```ini
host    all    all    0.0.0.0/0    scram-sha-256
```

Then, restart PostgreSQL for the changes to take effect:
```sh
sudo systemctl restart postgresql
```

## Step 2: Clone the Repository
```sh
git clone https://github.com/pgmoneta/pgmoneta.git
cd pgmoneta
```

## Step 3: Build the Docker Image

There are two Dockerfiles available:
1. **Alpine-based image**

**Using Docker**
```sh
docker build -t pgmoneta:latest -f ./contrib/docker/Dockerfile.alpine .
```

**Using Podman**

```sh
podman build -t pgmoneta:latest -f ./contrib/docker/Dockerfile.alpine .
```

2. **Rocky Linux 9-based image**

**Using Docker**
```sh
docker build -t pgmoneta:latest -f ./contrib/docker/Dockerfile.rocky9 .
```

**Using Podman**

```sh
podman build -t pgmoneta:latest -f ./contrib/docker/Dockerfile.rocky9 .
```

## Step 4: Run pgmoneta as a Docker Container

Once the image is built, run the container using:

- **Using Docker**

```sh
docker run -d --name pgmoneta --network host pgmoneta:latest
```

- **Using Podman**

```sh
podman run -d --name pgmoneta --network host pgmoneta:latest
```

## Step 5: Verify the Container

Check if the container is running:

- **Using Docker**

```sh
docker ps | grep pgmoneta -->
```

- **Using Podman**
```sh
podman ps | grep pgmoneta
```

Check logs for any errors:

- **Using Docker**

```sh
docker logs pgmoneta
```

- **Using Podman**

```sh
podman logs pgmoneta
```

You can also inspect the exposed metrics at:
```
http://localhost:5001/metrics
```

You can stop the container using

- **Using Docker**

```sh
docker stop pgmoneta
```

- **Using Podman**

```sh
podman stop pgmoneta
```

You can exec into the container and run the cli commands as

```sh
docker exec -it pgmoneta /bin/bash
#or using podman
podman exec -it pgmoneta /bin/bash

cd /etc/pgmoneta
/usr/local/bin/pgmoneta-cli -c pgmoneta.conf shutdown
```

See [this](https://github.com/pgmoneta/pgmoneta/blob/main/doc/manual/user-10-cli.md) for more cli commands.

You can access the three binaries at `/usr/local/bin`
