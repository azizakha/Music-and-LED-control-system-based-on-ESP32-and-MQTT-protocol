import telebot
import speech_recognition as sr
import paho.mqtt.client as mqtt
import urllib.request
import os
from pydub import AudioSegment
from telebot.handler_backends import State, StatesGroup
from telebot.storage import StateMemoryStorage

# StateMemoryStorage bilan botni ishga tushirish
state_storage = StateMemoryStorage()
bot = telebot.TeleBot("6011925083:AAH2YhbBYjzDjie3TS_SaN53inqeIzFfgGU", state_storage=state_storage)
# Webhookni o'chirish

# MQTT sozlamalari
MQTT_BROKER = "broker.emqx.io"
MQTT_PORT = 1883
MQTT_TOPIC = "esp32/buttons"
mqtt_client = mqtt.Client(protocol=mqtt.MQTTv311)

# Ovozli xabarni tanib olish uchun
recognizer = sr.Recognizer()

# MQTT ulanishi
def on_connect(client, userdata, flags, rc):
    print("MQTT brokeriga ulandi: " + str(rc))

mqtt_client.on_connect = on_connect
mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)
mqtt_client.loop_start()

# Ovozli xabarni matn sifatida tanib olish
def voice_to_text(audio_file):
    wav_file = "voice_message.wav"
    try:
        audio = AudioSegment.from_file(audio_file, format="ogg")
        audio.export(wav_file, format="wav")
    except Exception as e:
        print(f"Konvertatsiya xatosi: {e}")
        return "Faylni konvertatsiya qilishda xatolik"

    try:
        with sr.AudioFile(wav_file) as source:
            audio_data = recognizer.record(source)
            try:
                text = recognizer.recognize_google(audio_data, language="en-US")
                return text.lower()
            except sr.UnknownValueError:
                return "Ovoz aniqlanmadi"
            except sr.RequestError:
                return "Internet ulanishida xatolik"
    finally:
        if os.path.exists(wav_file):
            os.remove(wav_file)

# /start buyrug‘i uchun handler
@bot.message_handler(commands=['start'])
def send_welcome(message):
    bot.reply_to(message, "Salom! Ovozli xabar yuboring (masalan, 'first music') yoki matn yozing.")

# Ovozli xabarlar uchun handler
@bot.message_handler(content_types=['voice'])
def handle_voice(message):
    file_info = bot.get_file(message.voice.file_id)
    file_url = f"https://api.telegram.org/file/bot{bot.token}/{file_info.file_path}"
    audio_file = "voice_message.ogg"
    urllib.request.urlretrieve(file_url, audio_file)

    text = voice_to_text(audio_file)
    bot.reply_to(message, f"Tanib olindi: {text}")

    if "first music" in text:
        mqtt_client.publish(MQTT_TOPIC, "first_music")
        bot.reply_to(message, "First music chalinmoqda!")
    elif "second music" in text:
        mqtt_client.publish(MQTT_TOPIC, "second_music")
        bot.reply_to(message, "Second music chalinmoqda!")
    elif "third music" in text:
        mqtt_client.publish(MQTT_TOPIC, "third_music")
        bot.reply_to(message, "Third music chalinmoqda!")
    else:
        bot.reply_to(message, "Buyruq topilmadi. Iltimos, 'first music', 'second music' yoki 'third music' deb ayting.")

    if os.path.exists(audio_file):
        os.remove(audio_file)

# Matnli xabarlar uchun handler
@bot.message_handler(func=lambda message: True)
def handle_text(message):
    text = message.text.lower()
    if "first music" in text:
        mqtt_client.publish(MQTT_TOPIC, "first_music")
        bot.reply_to(message, "First music chalinmoqda!")
    elif "second music" in text:
        mqtt_client.publish(MQTT_TOPIC, "second_music")
        bot.reply_to(message, "Second music chalinmoqda!")
    elif "third music" in text:
        mqtt_client.publish(MQTT_TOPIC, "third_music")
        bot.reply_to(message, "Third music chalinmoqda!")
    else:
        bot.reply_to(message, "Buyruq topilmadi. 'first music', 'second music' yoki 'third music' deb yozing.")

# Botni ishga tushirish
if __name__ == "__main__":
    print("Bot ishga tushdi...")
    bot.infinity_polling()
