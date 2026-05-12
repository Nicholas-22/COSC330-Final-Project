#!/usr/bin/env python3
"""
EC2 -> S3 (local: MinIO): create a file, upload it, list the bucket, download it back.
Credentials are read from environment variables — never hardcoded.

On real EC2: remove endpoint_url and credential args; boto3 uses the IAM role automatically.
For local testing: run MinIO and set MINIO_ACCESS_KEY / MINIO_SECRET_KEY in your shell.
"""

import boto3
import botocore
import datetime
import platform
import socket
import os
import sys

BUCKET_NAME   = "cosc330-bucket"
UPLOAD_KEY    = "output.txt"
DOWNLOAD_NAME = "downloaded_output.txt"
LOCAL_FILE    = "output.txt"

MINIO_ENDPOINT   = os.environ.get("MINIO_ENDPOINT",    "http://localhost:9000")
MINIO_ACCESS_KEY = os.environ.get("MINIO_ACCESS_KEY",  "minioadmin")
MINIO_SECRET_KEY = os.environ.get("MINIO_SECRET_KEY",  "minioadmin")


def make_client():
    return boto3.client(
        "s3",
        endpoint_url=MINIO_ENDPOINT,
        aws_access_key_id=MINIO_ACCESS_KEY,
        aws_secret_access_key=MINIO_SECRET_KEY,
        region_name="us-east-1",
        config=botocore.client.Config(signature_version="s3v4"),
    )


def create_file(path):
    now      = datetime.datetime.utcnow().isoformat() + "Z"
    hostname = socket.gethostname()
    uname    = platform.uname()
    content  = (
        f"Timestamp : {now}\n"
        f"Hostname  : {hostname}\n"
        f"System    : {uname.system} {uname.release}\n"
        f"Machine   : {uname.machine}\n"
    )
    with open(path, "w") as f:
        f.write(content)
    print(f"[1] Created '{path}':\n{content}")


def ensure_bucket(s3):
    try:
        s3.create_bucket(Bucket=BUCKET_NAME)
        print(f"[*] Bucket '{BUCKET_NAME}' created.")
    except s3.exceptions.BucketAlreadyOwnedByYou:
        print(f"[*] Bucket '{BUCKET_NAME}' already exists.")
    except Exception:
        pass  # some S3-compatible servers raise a different error on conflict


def upload_file(s3, path, key):
    s3.upload_file(path, BUCKET_NAME, key)
    print(f"[2] Uploaded '{path}' -> s3://{BUCKET_NAME}/{key}")


def list_bucket(s3):
    response = s3.list_objects_v2(Bucket=BUCKET_NAME)
    print(f"\n[3] Contents of s3://{BUCKET_NAME}:")
    for obj in response.get("Contents", []):
        print(f"    {obj['Key']}  ({obj['Size']} bytes,  {obj['LastModified']})")


def download_file(s3, key, dest):
    s3.download_file(BUCKET_NAME, key, dest)
    print(f"\n[4] Downloaded s3://{BUCKET_NAME}/{key} -> '{dest}'")


def main():
    s3 = make_client()
    create_file(LOCAL_FILE)
    try:
        ensure_bucket(s3)
        upload_file(s3, LOCAL_FILE, UPLOAD_KEY)
        list_bucket(s3)
        download_file(s3, UPLOAD_KEY, DOWNLOAD_NAME)
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)
    print("\nDone.")


if __name__ == "__main__":
    main()
