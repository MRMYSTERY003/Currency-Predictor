from flask import Flask, request, jsonify
import base64
from PIL import Image
import time
from io import BytesIO
import cv2
import numpy as np

from keras.models import load_model

mobilenet = load_model("model\\mobilenet")


app = Flask(__name__)
i = 1


def predict_base64(encoded_image):
    cls = ["50", "20", "10", "500", "200", "100"]

    # Decode base64 image
    decoded_image = base64.b64decode(encoded_image)
    pil_image = Image.open(BytesIO(decoded_image))

    # Convert to OpenCV format
    img_data = cv2.cvtColor(np.array(pil_image), cv2.COLOR_RGB2BGR)
    img_resized = cv2.resize(img_data, (224, 224))

    x = img_resized / 255.0
    x = x.reshape(-1, 224, 224, 3)
    res = mobilenet.predict(x)
    return cls[np.argmax(res)]


@app.route("/upload_image", methods=["GET", "POST", "PUT"])
def upload_image():
    try:
        global i
        # Get the base64-encoded image from the GET request
        base64_image = request.get_json()["img"]
        print(base64_image)

        # print(base64_image)

        res = predict_base64(base64_image)
        print(res)
        # Decode the base64 data to image
        # image_data = base64.b64decode(base64_image)
        # image = Image.open(BytesIO(image_data))

        # Save the image to the local storage (adjust the path as needed)
        # image.save(f"images/500/image{i}.jpg")
        # print(f"saved image at images/image{i}.jpg")
        # i += 1

        # time.sleep(1)

        return jsonify(f"key:{res}")
    except Exception as e:
        print(e)
        return jsonify({"success": False, "message": f"Error: {str(e)}"})


if __name__ == "__main__":
    app.run(debug=True, host="0.0.0.0")
