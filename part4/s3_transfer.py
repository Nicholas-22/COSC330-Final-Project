#!/usr/bin/env python3
"""
EC2 -> S3: create a file, upload it, list the bucket, then download it back.
Credentials come from the EC2 instance's IAM role — no hardcoded keys.
"""

import boto3
import datetime
import platform
import socket
import os
import sys

BUCKET_NAME      = "cosc330-final-bucket"   # change to your bucket name
UPLOAD_KEY       = "output.txt"
DOWNLOAD_NAME    = "downloaded_output.txt"
LOCAL_FILE       = "output.txt"


def create_file(path: str) -> None:
    now       = datetime.datetime.utcnow().isoformat() + "Z"
    hostname  = socket.gethostname()
    uname     = platform.uname()
    content   = (
        f"Timestamp : {now}\n"
        f"Hostname  : {hostname}\n"
        f"System    : {uname.system} {uname.release}\n"
        f"Machine   : {uname.machine}\n"
        f"Processor : {uname.processor}\n"
    )
    with open(path, "w") as f:
        f.write(content)
    print(f"[1] Created '{path}':\n{content}")


def upload_file(s3, path: str, key: str) -> None:
    s3.upload_file(path, BUCKET_NAME, key)
    print(f"[2] Uploaded '{path}' -> s3://{BUCKET_NAME}/{key}")


def list_bucket(s3) -> None:
    response = s3.list_objects_v2(Bucket=BUCKET_NAME)
    print(f"\n[3] Contents of s3://{BUCKET_NAME}:")
    for obj in response.get("Contents", []):
        print(f"    {obj['Key']}  ({obj['Size']} bytes,  {obj['LastModified']})")


def download_file(s3, key: str, dest: str) -> None:
    s3.download_file(BUCKET_NAME, key, dest)
    print(f"\n[4] Downloaded s3://{BUCKET_NAME}/{key} -> '{dest}'")


def main():
    # boto3 automatically uses the IAM role attached to the EC2 instance
    s3 = boto3.client("s3")

    create_file(LOCAL_FILE)

    try:
        upload_file(s3, LOCAL_FILE, UPLOAD_KEY)
        list_bucket(s3)
        download_file(s3, UPLOAD_KEY, DOWNLOAD_NAME)
    except s3.exceptions.NoSuchBucket:
        print(f"Error: bucket '{BUCKET_NAME}' does not exist. "
              "Create it first or update BUCKET_NAME.", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)

    print("\nDone.")


if __name__ == "__main__":
    main()
