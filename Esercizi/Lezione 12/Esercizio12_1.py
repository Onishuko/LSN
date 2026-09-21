#!/usr/bin/env python
# coding: utf-8

# <h1 style="color:red;">Numerical Simulation Laboratory - Exercise 12.1</h1>
# 
# ### MNIST with Keras
# 
# The goal of exercise 12 is to use deep neural network models, implemented in the Keras python package, to recognize and distinguish between the ten handwritten digits (0-9).
# 
# The MNIST dataset comprises $70000$ handwritten digits, each of which comes in a square image, divided into a $28\times 28$ pixel grid. Every pixel can take on $256$ gradation of the gray color, interpolating between white and black, and hence each data point assumes any value in the set $\{0,1,\dots,255\}$. Since there are $10$ categories in the problem, corresponding to the ten digits, this problem represents a generic **classification task**. 

# In[20]:


import tensorflow as tf
from tensorflow import keras
import numpy as np
import matplotlib.pyplot as plt
from keras.models import Sequential
from keras.layers import Dense, Dropout, Input
from keras.optimizers import SGD, Adam, RMSprop
import pandas as pd


# In[2]:


seed = 0
np.random.seed(seed)
tf.random.set_seed(seed)


# In[3]:


from keras.datasets import mnist

# Input image dimensions
img_rows, img_cols = 28, 28

# Number of classes
num_classes = 10

# Load MNIST dataset
(X_train, Y_train), (X_test, Y_test) = mnist.load_data()

print("X_train shape:", X_train.shape)
print("Y_train shape:", Y_train.shape)
print("X_test shape:", X_test.shape)
print("Y_test shape:", Y_test.shape)


# In[4]:


# Flatten images
X_train = X_train.reshape(X_train.shape[0], img_rows * img_cols)
X_test = X_test.reshape(X_test.shape[0], img_rows * img_cols)

print("X_train shape:", X_train.shape)
print("X_test shape:", X_test.shape)


# In[5]:


# Convert to float32
X_train = X_train.astype("float32")
X_test = X_test.astype("float32")

# Rescale pixel values to [0, 1]
X_train /= 255.0
X_test /= 255.0

print("Minimum pixel value:", X_train.min())
print("Maximum pixel value:", X_train.max())


# In[6]:


print("Example label:", Y_train[50])

plt.matshow(X_train[50].reshape(28, 28), cmap="binary")
plt.show()


# In[7]:


Y_train = keras.utils.to_categorical(Y_train, num_classes)
Y_test = keras.utils.to_categorical(Y_test, num_classes)

print("Y_train shape:", Y_train.shape)
print("Y_test shape:", Y_test.shape)


# In[8]:


def create_DNN():
    model = Sequential()

    # Input layer
    model.add(Input(shape=(img_rows * img_cols,)))

    # First hidden layer
    model.add(Dense(400, activation="relu"))

    # Second hidden layer
    model.add(Dense(100, activation="relu"))

    # Dropout for regularization
    model.add(Dropout(0.5))

    # Output layer
    model.add(Dense(num_classes, activation="softmax"))

    return model


# In[9]:


model = create_DNN()
model.summary()


# In[10]:


def compile_model(optimizer):
    model = create_DNN()

    model.compile(
        loss=keras.losses.categorical_crossentropy,
        optimizer=optimizer,
        metrics=["accuracy"]
    )

    return model


# In[11]:


batch_size = 32
epochs = 40


# In[12]:


model_SGD = compile_model(SGD())
model_Adam = compile_model(Adam())
model_RMSprop = compile_model(RMSprop())


# In[13]:


history_SGD = model_SGD.fit(
    X_train,
    Y_train,
    batch_size=batch_size,
    epochs=epochs,
    shuffle=True,
    verbose=1,
    validation_data=(X_test, Y_test)
)


# In[14]:


history_Adam = model_Adam.fit(
    X_train,
    Y_train,
    batch_size=batch_size,
    epochs=epochs,
    shuffle=True,
    verbose=1,
    validation_data=(X_test, Y_test)
)


# In[15]:


history_RMSprop = model_RMSprop.fit(
    X_train,
    Y_train,
    batch_size=batch_size,
    epochs=epochs,
    shuffle=True,
    verbose=1,
    validation_data=(X_test, Y_test)
)


# In[16]:


score_SGD = model_SGD.evaluate(X_test, Y_test, verbose=0)
score_Adam = model_Adam.evaluate(X_test, Y_test, verbose=0)
score_RMSprop = model_RMSprop.evaluate(X_test, Y_test, verbose=0)


# In[17]:


print("SGD:")
print("  Test loss:", score_SGD[0])
print("  Test accuracy:", score_SGD[1])

print("\nAdam:")
print("  Test loss:", score_Adam[0])
print("  Test accuracy:", score_Adam[1])

print("\nRMSprop:")
print("  Test loss:", score_RMSprop[0])
print("  Test accuracy:", score_RMSprop[1])


# In[18]:


plt.figure(figsize=(10, 6))

plt.plot(history_SGD.history["accuracy"], label="SGD - train")
plt.plot(history_SGD.history["val_accuracy"], label="SGD - validation")

plt.plot(history_Adam.history["accuracy"], label="Adam - train")
plt.plot(history_Adam.history["val_accuracy"], label="Adam - validation")

plt.plot(history_RMSprop.history["accuracy"], label="RMSprop - train")
plt.plot(history_RMSprop.history["val_accuracy"], label="RMSprop - validation")

plt.xlabel("Epoch")
plt.ylabel("Accuracy")
plt.title("Training and validation accuracy")
plt.legend()
plt.grid()
plt.show()


# In[19]:


plt.figure(figsize=(10, 6))

plt.plot(history_SGD.history["loss"], label="SGD - train")
plt.plot(history_SGD.history["val_loss"], label="SGD - validation")

plt.plot(history_Adam.history["loss"], label="Adam - train")
plt.plot(history_Adam.history["val_loss"], label="Adam - validation")

plt.plot(history_RMSprop.history["loss"], label="RMSprop - train")
plt.plot(history_RMSprop.history["val_loss"], label="RMSprop - validation")

plt.xlabel("Epoch")
plt.ylabel("Loss")
plt.title("Training and validation loss")
plt.legend()
plt.grid()
plt.show()


# In[21]:


results = pd.DataFrame({
    "Optimizer": ["SGD", "Adam", "RMSprop"],
    "Test loss": [
        score_SGD[0],
        score_Adam[0],
        score_RMSprop[0]
    ],
    "Test accuracy": [
        score_SGD[1],
        score_Adam[1],
        score_RMSprop[1]
    ]
})

results


# ### Conclusion
# 
# The three optimizers achieve a test accuracy of about 98%, with Adam reaching the highest final accuracy (98.32%). However, SGD shows a significantly lower test loss and a more stable validation behavior. In particular, the validation loss for SGD decreases throughout the training, whereas Adam and RMSprop exhibit stronger fluctuations and an overall increasing trend, suggesting a tendency towards overfitting. Therefore, for the chosen network architecture and hyperparameters, SGD provides the most stable generalization performance, although Adam achieves a slightly higher final accuracy.

# In[25]:


predictions = model_SGD.predict(X_test)

indices = np.random.choice(len(X_test), 10, replace=False)

plt.figure(figsize=(15, 6))

for i, index in enumerate(indices):
    ax = plt.subplot(2, 5, i + 1)

    plt.imshow(
        X_test[index].reshape(28, 28),
        cmap="gray"
    )

    true_label = np.argmax(Y_test[index])
    predicted_label = np.argmax(predictions[index])

    plt.title(
        f"True: {true_label}\nPredicted: {predicted_label}"
    )

    plt.axis("off")

plt.tight_layout()
plt.show()


# In[27]:


#Seraching for wrong predictions

predicted_labels = np.argmax(predictions, axis=1)
true_labels = np.argmax(Y_test, axis=1)

wrong_indices = np.where(predicted_labels != true_labels)[0]

print("Number of misclassified images:", len(wrong_indices))

plt.figure(figsize=(15, 6))

for i in range(10):
    index = wrong_indices[i]

    ax = plt.subplot(2, 5, i + 1)

    plt.imshow(X_test[index].reshape(28, 28), cmap="gray")

    plt.title(
        f"True: {true_labels[index]}\n"
        f"Predicted: {predicted_labels[index]}"
    )

    plt.axis("off")

plt.tight_layout()
plt.show()


# In[ ]:




