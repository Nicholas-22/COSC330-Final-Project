# COSC 330 – Final Project

## Directory Layout

```
part1/   mylib.c/h  shell.c  Makefile   (static + shared library + shell)
part2/   myshell.service                (systemd unit file)
part3/   server.c  client.c  Makefile   (EC2-to-EC2 TCP file transfer)
part4/   s3_transfer.py                 (EC2-to-S3 upload/download)
```

---

## Part 1 – Build & Run

```bash
cd part1
make          # builds libmylib.a, libmylib.so, and shell
./shell
```

`-Wl,-rpath,.` tells the linker to look for `libmylib.so` in the current directory,
so no `LD_LIBRARY_PATH` export is needed when running from `part1/`.

To use the **static** library instead (for testing):
```bash
gcc -Wall -o shell_static shell.c -L. -l:libmylib.a
```

---

## Part 2 – systemd Service

```bash
# Copy binary and library to EC2
scp -i key.pem part1/shell part1/libmylib.so ec2-user@<EC2-IP>:~/part1/

# Install the service
sudo cp part2/myshell.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl start  myshell
sudo systemctl enable myshell   # start on boot
sudo systemctl status myshell
sudo journalctl -u myshell -f   # live logs
sudo systemctl stop   myshell
sudo systemctl disable myshell
```

---

## Part 3 – EC2-to-EC2 File Transfer

Build on **both** EC2 instances:
```bash
cd part3
make
```

On the **server** EC2:
```bash
./server
```

On the **client** EC2 (same security group, port 9000 open inbound):
```bash
./client <server-private-IP> file1.txt
```

The server saves the incoming file as `received_file1.txt`.

---

## Part 4 – EC2 to S3

1. Attach an IAM role to your EC2 with policy permissions:
   `s3:PutObject`, `s3:GetObject`, `s3:ListBucket`
2. Edit `BUCKET_NAME` in `s3_transfer.py` to match your bucket.
3. Run:

```bash
cd part4
pip install boto3          # if not already installed
python3 s3_transfer.py
```
