from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as ec
from selenium.webdriver.common.keys import Keys
import time
from getpass import getpass


browser= webdriver.Chrome()
browser.implicitly_wait(20)
list=["cream", "soap", "shampoo"]
win_before = browser.window_handles[0]
browser.get("https://www.amazon.in/")


browser.maximize_window()
browser.find_element_by_xpath("""/html/body/div[1]/header/div/div[1]/div[2]/div/a[2]/div/span""").click()
browser.find_element_by_xpath("""/html/body/div[1]/div[1]/div[2]/div/div[2]/div/div[1]/form/div/div/div/div[1]/input[1]""").send_keys("smathur725@gmail.com")
browser.find_element_by_xpath("/html/body/div[1]/div[1]/div[2]/div/div[2]/div/div[1]/form/div/div/div/div[2]/span/span/input").click()
password= browser.find_element_by_xpath("""/html/body/div[1]/div[1]/div[2]/div/div[2]/div[1]/div/div/form/div/div[1]/input""")
password.send_keys(getpass("Enter your Amazon password: "))
browser.find_element_by_xpath("""/html/body/div[1]/div[1]/div[2]/div/div[2]/div[1]/div/div/form/div/div[2]/span/span/input""").click()
#login done
c=1
for i in list:
    browser.find_element_by_xpath("""/html/body/div[1]/header/div/div[1]/div[3]/div/form/div[3]/div[1]/input""").send_keys(i)
    browser.find_element_by_xpath("""/html/body/div[1]/header/div/div[1]/div[3]/div/form/div[2]/div/input""").click()
    elems=browser.find_elements_by_xpath("""/html/body/div[1]/div[2]/div[1]/div[2]/div/span[3]/div[2]/div[2]/div/span/div/div/div/div/div[2]/h2/a/span""")
    elems[0].click()
    time.sleep(3)
    win_after = browser.window_handles[c]
    browser.switch_to.window(win_after)
    time.sleep(3)
    browser.find_element_by_xpath("""/html/body/div[2]/div[2]/div[4]/div[5]/div[1]/div[3]/div/div/div/form/div/div/div/div/div[2]/div/div[30]/div[1]/span/span/span/input""").click()
    browser.find_element_by_xpath("""/html/body/div[1]/div/header/div/div[1]/div[1]/div/a[1]/span[1]""").click()
    c=c+1
time.sleep(3)    
browser.find_element_by_xpath("""/html/body/div[1]/header/div/div[1]/div[2]/div/a[6]/span[3]""").click()
time.sleep(3)
browser.find_element_by_xpath("""/html/body/div[1]/div[4]/div/div[5]/div/div[1]/div[3]/form/div/div[3]/div/span/span/input""").click()
browser.find_element_by_xpath("""/html/body/div[5]/div[2]/div/div[2]/form[1]/div/div[1]/div[2]/span/a""").click()
browser.find_element_by_xpath("""/html/body/div[5]/div[1]/div/div[2]/div/div[1]/form/div[1]/div[2]/div/span[1]/span/input""").click()

