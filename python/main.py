import pyheif
from PIL import Image
import sys

def convert_heic_to_jpeg(heic_path, output_path):
    # Read the HEIC file
    heif_file = pyheif.read(heic_path)

    # Convert HEIC to PIL Image
    image = Image.frombytes(
        heif_file.mode, 
        heif_file.size, 
        heif_file.data,
        "raw",
        heif_file.mode,
        heif_file.stride,
    )

    # Save as JPEG
    image.save(output_path, format="JPEG", quality=90)

    print(f"Conversion successful. File saved as {output_path}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py input.heic output.jpg")
        sys.exit(1)

    heic_path = sys.argv[1]
    output_path = sys.argv[2]
    convert_heic_to_jpeg(heic_path, output_path)
