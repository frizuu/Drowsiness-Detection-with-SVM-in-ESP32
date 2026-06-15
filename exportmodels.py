from micromlgen import port
import joblib

model = joblib.load("svm_model.pkl")

with open("model.h", "w") as file:
    file.write(port(model))