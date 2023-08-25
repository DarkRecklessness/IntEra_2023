import socket
# import time

HOST = ''  # Server IP or Hostname # '192.168.37.63'
PORT = 6969  # Pick an open Port (1000+ recommended, 65xxx > port), must match the client sport

LOW_TEMP = 22
HIGH_TEMP = 26

SECOND = "10.42.0.215"
FIRST = "10.42.0.245"


def calibrate_temp(now_temp, prev_temp):
    if now_temp > prev_temp:
        return round(min(now_temp, prev_temp + 0.87) * 100) / 100
    else:
        return round(max(now_temp, prev_temp - 0.87) * 100) / 100


def procent_fan(temp_inside, temp_outside):
    procent = min(temp_inside - HIGH_TEMP, 3) / 0.03  # Если разница больше 5-ух, то скорость 100%
    return procent


s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((HOST, PORT))
s.listen(2)
temp_inside = temp_outside = 20
# mas, n = ["S1 F1", "S0 F20", "S1 F40", "S0 F60", "S1 F80", "S0 F100"], 0
while True:
    conn, addr = s.accept()
    data = str(conn.recv(1024))[2:-1]
    # print(data)
    if addr[0] == FIRST:
        temp_inside = calibrate_temp(float(data), temp_inside)
        print(f'Inside: {temp_inside}')
    else:
        temp_outside = calibrate_temp(float(data), temp_outside)
        print(f'Outside: {temp_outside}')

    # if addr[0] == FIRST:
    # conn.send(mas[n].encode('utf-8'))
    #     if addr[0] == FIRST:
    #         if abs(temp_inside - temp_outside) <= 1:
    #             conn.send(f'S0 F0'.encode('utf-8'))
    #         elif temp_inside > temp_outside:
    #             conn.send(f'S0 F{procent_fan(temp_inside, temp_outside)}'.encode('utf-8'))
    #         else:
    #             conn.send(f'S1 F0'.encode('utf-8')) #поменять на S1
    ###################################################
    # Алгоритм по заданию
    if addr[0] == FIRST:
        if LOW_TEMP <= temp_inside <= HIGH_TEMP or abs(temp_inside - temp_outside) <= 1:
            conn.send(f'S0 F0'.encode('utf-8'))
        elif temp_inside > HIGH_TEMP and temp_inside > temp_outside:
            conn.send(f'S0 F{procent_fan(temp_inside, temp_outside)}'.encode('utf-8'))
        elif temp_inside < LOW_TEMP and temp_outside > temp_inside:
            conn.send(f'S1 F0'.encode('utf-8'))

    ##################################################
#     n += 1
#     if n == 6:
#         n = 0
# time.sleep(5)
# conn.close() # Close connections

