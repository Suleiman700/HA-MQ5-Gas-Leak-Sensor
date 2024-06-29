import logging
import requests
import voluptuous as vol

from homeassistant.components.sensor import PLATFORM_SCHEMA
from homeassistant.const import CONF_NAME, CONF_SCAN_INTERVAL
from homeassistant.helpers.entity import Entity
import homeassistant.helpers.config_validation as cv
from datetime import timedelta

_LOGGER = logging.getLogger(__name__)

DEFAULT_NAME = "Gas Sensor"
DEFAULT_SCAN_INTERVAL = timedelta(seconds=10)
CONF_API_PATH = "api_path"

PLATFORM_SCHEMA = PLATFORM_SCHEMA.extend({
    vol.Required(CONF_API_PATH): cv.string,
    vol.Optional(CONF_NAME, default=DEFAULT_NAME): cv.string,
    vol.Optional(CONF_SCAN_INTERVAL, default=DEFAULT_SCAN_INTERVAL): cv.time_period,
})

def setup_platform(hass, config, add_entities, discovery_info=None):
    name = config.get(CONF_NAME)
    api_path = config.get(CONF_API_PATH)
    scan_interval = config.get(CONF_SCAN_INTERVAL)

    add_entities([GasSensor(name, api_path, scan_interval)], True)

class GasSensor(Entity):

    def __init__(self, name, api_path, scan_interval):
        self._name = name
        self._api_path = api_path
        self._scan_interval = scan_interval
        self._state = None

    @property
    def name(self):
        return self._name

    @property
    def state(self):
        return self._state

    @property
    def unit_of_measurement(self):
        return "ppm"

    @property
    def icon(self):
        return "mdi:smoke"

    def update(self):
        try:
            response = requests.get(self._api_path, timeout=10)
            response.raise_for_status()
            self._state = float(response.text)
        except requests.exceptions.RequestException as e:
            _LOGGER.error("Error fetching data: %s", e)
            self._state = None